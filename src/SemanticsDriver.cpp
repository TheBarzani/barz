#include "Semantics/SymbolTableVisitor.h"
#include "Semantics/SemanticCheckingVisitor.h"
#include "Parser/Parser.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options] file1 [file2 ...]\n"
              << "Options:\n"
              << "  -t, --table <csv_file>   Specify a custom parsing table CSV file. Default is 'attribute_grammar_parsing_table.csv'.\n"
              << "  -o, --output <dir>       Specify output directory for symbol tables. Default is current directory.\n"
              << "  -h, --help               Show this help message.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string tableFile = "attribute_grammar_parsing_table.csv";
    std::string outputDir = ".";
    std::vector<std::string> inputFiles;

    for (int i = 1; i < argc; ) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-t" || arg == "--table") {
            if (i + 1 < argc) {
                tableFile = argv[i + 1];
                i += 2;
            } else {
                std::cerr << "Error: --table requires a CSV file argument.\n";
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputDir = argv[i + 1];
                i += 2;
            } else {
                std::cerr << "Error: --output requires a directory argument.\n";
                return 1;
            }
        } else {
            inputFiles.push_back(arg);
            i++;
        }
    }

    if (inputFiles.empty()) {
        std::cerr << "Error: No input files provided.\n";
        printUsage(argv[0]);
        return 1;
    }

    // Ensure output directory exists
    if (!fs::exists(outputDir)) {
        std::cout << "Creating output directory: " << outputDir << std::endl;
        fs::create_directories(outputDir);
    }

    for (const auto& file : inputFiles) {
        std::cout << "Processing file: " << file << std::endl;
        
        // First parse the file to generate AST
        std::cout << "Parsing file: " << file << " with table: " << tableFile << std::endl;
        Parser parser(file, tableFile);
        
        bool parseSuccess = parser.parse();
        if (!parseSuccess) {
            std::cerr << "Error: Failed to parse " << file << std::endl;
            continue;
        }
        
        // Get the AST from the parser
        AST& ast = parser.getAST();
        ASTNode* root = ast.getRoot();
        
        // Phase 1: Symbol Table Generation
        std::cout << "Generating symbol table..." << std::endl;
        SymbolTableVisitor symbolTableVisitor;
        root->accept(&symbolTableVisitor);
        
        // Output the symbol table to file
        std::string outputFile = file + ".outsymboltables";
        symbolTableVisitor.outputSymbolTable(outputFile);
        std::cout << "Symbol table generated: " << outputFile << std::endl;
        
        // Phase 2: Semantic Checking
        std::cout << "Performing semantic checking..." << std::endl;
        SemanticCheckingVisitor semanticChecker(symbolTableVisitor.getGlobalTable());

        // Import errors from the symbol table visitor
        semanticChecker.importSymbolTableErrors(symbolTableVisitor);

        // Continue with semantic checking
        root->accept(&semanticChecker);
        
        // Output semantic errors to file
        semanticChecker.outputErrors(file);

        if (semanticChecker.hasErrors()) {
            std::cout << "Semantic errors found. See " << file << ".outsemanticerrors for details." << std::endl;
        } else {
            std::cout << "No semantic errors found." << std::endl;
        }
        
        std::cout << "-------------------------------------------" << std::endl;
    }

    return 0;
}