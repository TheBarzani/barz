#include "Semantics/SymbolTableVisitor.h"
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
        Parser* parser = new Parser(file, tableFile);
        
        bool parseSuccess = parser->parse();
        if (!parseSuccess) {
            std::cerr << "Error: Failed to parse " << file << std::endl;
            continue;
        }
        
        // Get the AST from the parser
        // Get a reference to the AST - not a copy
        AST& ast = parser->getAST();
        // Now get the root from the reference
        ASTNode* root = ast.getRoot();
        // Write the AST to file using the reference
        // ast.writeToFile(inputFiles[0] + ".dot");

        // Now perform semantic analysis
        std::cout << "Generating symbol table..." << std::endl;
        
        // Create a symbol table visitor and traverse the AST
        SymbolTableVisitor visitor;
        root->accept(&visitor);

        std::cout << "Generating symbol table..." << std::endl;
        
        // Get the base filename without path and extension
        fs::path filePath(file);
        std::string baseName = filePath.stem().string();
        
        // Create output filename with .outsymboltables extension
        std::string outputFile = outputDir + "/" + baseName + ".outsymboltables";
        
        // Output the symbol table to file
        visitor.outputSymbolTable(outputFile);
        
        std::cout << "Symbol table generated: " << outputFile << std::endl;
        std::cout << "-------------------------------------------" << std::endl;
    }

    return 0;
}