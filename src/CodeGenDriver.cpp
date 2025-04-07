#include "Semantics/SymbolTableVisitor.h"
#include "Semantics/SemanticCheckingVisitor.h"
#include "CodeGenerator/MemSizeVisitor.h"
#include "Parser/Parser.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options] file\n"
              << "Options:\n"
              << "  -t, --table <csv_file>   Specify a custom parsing table CSV file. Default is 'attribute_grammar_parsing_table.csv'.\n"
              << "  -o, --output <dir>       Specify output directory for generated files. Default is current directory.\n"
              << "  -h, --help               Show this help message.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string tableFile = "attribute_grammar_parsing_table.csv";
    std::string outputDir = ".";
    std::string inputFile;

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
            inputFile = arg;
            i++;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: No input file provided.\n";
        printUsage(argv[0]);
        return 1;
    }

    // Ensure output directory exists
    if (!fs::exists(outputDir)) {
        std::cout << "Creating output directory: " << outputDir << std::endl;
        fs::create_directories(outputDir);
    }

    std::cout << "Processing file: " << inputFile << std::endl;

    // First parse the file to generate AST
    std::cout << "Parsing file: " << inputFile << " with table: " << tableFile << std::endl;
    Parser parser(inputFile, tableFile);

    bool parseSuccess = parser.parse();
    if (!parseSuccess) {
        std::cerr << "Error: Failed to parse " << inputFile << std::endl;
        return 1;
    }

    // Get the AST from the parser
    AST& ast = parser.getAST();
    ASTNode* root = ast.getRoot();

    // Phase 1: Symbol Table Generation
    std::cout << "Generating symbol table..." << std::endl;
    SymbolTableVisitor symbolTableVisitor;
    root->accept(&symbolTableVisitor);

    // Output the symbol table to file
    std::string symbolTableFile = inputFile + ".outsymboltables";
    symbolTableVisitor.outputSymbolTable(symbolTableFile);
    std::cout << "Symbol table generated: " << symbolTableFile << std::endl;

    // Phase 2: Semantic Checking
    std::cout << "Performing semantic checking..." << std::endl;
    SemanticCheckingVisitor semanticChecker(symbolTableVisitor.getGlobalTable());

    // Import errors from the symbol table visitor
    semanticChecker.importSymbolTableErrors(symbolTableVisitor);

    // Continue with semantic checking
    root->accept(&semanticChecker);

    // Output semantic errors to file
    std::string semanticErrorsFile = inputFile;
    semanticChecker.outputErrors(semanticErrorsFile);

    if (semanticChecker.hasErrors()) {
        std::cout << "Semantic errors found. See " << semanticErrorsFile << " for details." << std::endl;
        return 1;
    } else {
        std::cout << "No semantic errors found." << std::endl;
    }

    // Phase 3: Memory Size Calculation
    std::cout << "Calculating memory sizes and offsets..." << std::endl;
    MemSizeVisitor memSizeVisitor(symbolTableVisitor.getGlobalTable());
    
    // Process the AST to identify temporary variables and expressions
    memSizeVisitor.processAST(root);
    
    // Calculate memory sizes and offsets for all symbols
    memSizeVisitor.calculateMemorySizes();
    
    // Output the updated symbol table with memory sizes
    std::string sizeSymbolTableFile = inputFile + ".sizesymboltable";
    memSizeVisitor.outputSymbolTable(sizeSymbolTableFile);
    std::cout << "Memory-sized symbol table generated: " << sizeSymbolTableFile << std::endl;

    // Phase 4: Code Generation (commented out for now)
    // std::cout << "Generating MOON machine code..." << std::endl;
    // MoonCodeGenerator codeGenerator(symbolTableVisitor.getGlobalTable());
    // root->accept(&codeGenerator);

    // // Output MOON code to file
    // std::string moonCodeFile = fs::path(outputDir) / (fs::path(inputFile).stem().string() + ".moon");
    // codeGenerator.outputMoonCode(moonCodeFile);
    // std::cout << "MOON machine code generated: " << moonCodeFile << std::endl;

    std::cout << "-------------------------------------------++" << std::endl;

    return 0;
}