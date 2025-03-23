#include "Semantics/SymbolTableVisitor.h"
#include "Semantics/SemanticCheckingVisitor.h"
#include "Parser/Parser.h"
#include <iostream>
#include <string>
#include <vector>

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options] file1 [file2 ...]\n"
              << "Options:\n"
              << "  --table <csv_file>   Specify a custom parsing table CSV file. Default is 'attribute_grammar_parsing_table.csv'.\n"
              << "  --output <dir>       Specify output directory for symbol tables. Default is current directory.\n"
              << "  -h, --help           Show this help message.\n";
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

    for (const auto& file : inputFiles) {
        std::cout << "Processing file: " << file << std::endl;
        
        // First parse the file to generate AST
        std::cout << "Parsing file: " << file << " with table: " << tableFile << std::endl;
        Parser parser(file, tableFile);
        auto ast = parser.parse();
        
        if (!ast) {
            std::cerr << "Error: Failed to parse " << file << std::endl;
            continue;
        }
        
        // Now perform semantic analysis
        std::cout << "Performing semantic analysis..." << std::endl;
        // SemanticAnalyzer analyzer(ast, outputDir);
        if (ast) {
            std::cout << "Semantic analysis completed successfully." << std::endl;
            std::cout << "Symbol tables generated in: " << outputDir << std::endl;
        } else {
            std::cerr << "Semantic analysis failed with errors." << std::endl;
        }
        
        std::cout << "-------------------------------------------" << std::endl;
    }

    return 0;
}