#include "Scanner/Scanner.h"
#include "Parser/Parser.h"
#include "Semantics/SymbolTableVisitor.h"
#include "Semantics/SemanticCheckingVisitor.h"
#include "CodeGenerator/MemSizeVisitor.h"
#include "CodeGenerator/CodeGenVisitor.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <algorithm>

namespace fs = std::filesystem;

enum class CompilerPhase {
    SCAN = 1,    // Lexical analysis
    PARSE = 2,   // Syntax analysis
    SYMBOL = 3,  // Symbol table generation
    SEMANTIC = 4, // Semantic analysis
    MEMORY = 5,  // Memory size allocation
    CODEGEN = 6  // Code generation
};

const std::map<std::string, CompilerPhase> phaseMap = {
    {"scan", CompilerPhase::SCAN},
    {"parse", CompilerPhase::PARSE},
    {"symbol", CompilerPhase::SYMBOL},
    {"semantic", CompilerPhase::SEMANTIC},
    {"memory", CompilerPhase::MEMORY},
    {"codegen", CompilerPhase::CODEGEN},
    {"1", CompilerPhase::SCAN},
    {"2", CompilerPhase::PARSE},
    {"3", CompilerPhase::SYMBOL},
    {"4", CompilerPhase::SEMANTIC},
    {"5", CompilerPhase::MEMORY},
    {"6", CompilerPhase::CODEGEN}
};

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options] file\n"
              << "Options:\n"
              << "  -t, --table <csv_file>   Specify a custom parsing table CSV file. Default is 'attribute_grammar_parsing_table.csv'.\n"
              << "  -o, --output <dir>       Specify output directory for generated files. Default is current directory.\n"
              << "  -p, --phase <phase>      Specify the compiler phase to run up to. Options:\n"
              << "                           scan (1): Lexical analysis only\n"
              << "                           parse (2): Syntax analysis\n"
              << "                           symbol (3): Symbol table generation\n"
              << "                           semantic (4): Semantic analysis\n" 
              << "                           memory (5): Memory allocation\n"
              << "                           codegen (6): Code generation (default)\n"
              << "  -h, --help               Show this help message.\n";
}

// Phase 1: Lexical Analysis
Scanner runScannerPhase(const std::string& inputFile) {
    std::cout << "\n=========Phase 1: Lexical Analysis=========" << std::endl;
    
    // Extract directory and filename
    fs::path inputPath(inputFile);
    fs::path directory = inputPath.parent_path();
    fs::path scannerOutDir;
    
    if (directory.empty()) {
        // If no directory specified, use current directory
        scannerOutDir = "scanner_out";
    } else {
        // Use the same directory as input file
        scannerOutDir = directory / "scanner_out";
    }
    
    // Create scanner_out directory if it doesn't exist
    if (!fs::exists(scannerOutDir)) {
        std::cout << "Creating scanner output directory: " << scannerOutDir << std::endl;
        fs::create_directories(scannerOutDir);
    }
    
    // Create output path for scanner files
    fs::path outputBase = scannerOutDir / inputPath.stem();
    std::string outputPath = outputBase.string();
    
    Scanner scanner(inputFile, outputPath);
    scanner.processFile();
    scanner.writeOutputsToFile();
    std::cout << "Lexical analysis completed" << std::endl;
    std::cout << "Scanner output files written to: " << scannerOutDir << std::endl;
    return scanner;
}

// Phase 2: Syntax Analysis and AST Construction
AST runParserPhase(const std::string& inputFile, const std::string& tableFile, Scanner& scanner) {
    std::cout << "\n=========Phase 2: Syntax Analysis=========" << std::endl;
    std::cout << "Parsing file: " << inputFile << " with table: " << tableFile << std::endl;
    
    // Extract directory and filename
    fs::path inputPath(inputFile);
    fs::path directory = inputPath.parent_path();
    fs::path parserOutDir;
    
    if (directory.empty()) {
        // If no directory specified, use current directory
        parserOutDir = "parser_out";
    } else {
        // Use the same directory as input file
        parserOutDir = directory / "parser_out";
    }
    
    // Create parser_out directory if it doesn't exist
    if (!fs::exists(parserOutDir)) {
        std::cout << "Creating parser output directory: " << parserOutDir << std::endl;
        fs::create_directories(parserOutDir);
    }
    
    // Create output path for parser files
    fs::path outputBase = parserOutDir / inputPath.stem();
    std::string outputPath = outputBase.string();
    
    Parser parser(inputFile, tableFile, scanner);
    bool parseSuccess = parser.parse();
    // Write parser output files to the parser_out directory
    parser.writeOutputFiles(outputPath);
    
    if (!parseSuccess) {
        std::cerr << "Error: Failed to parse " << inputFile << std::endl;
        AST emptyAST;
        return emptyAST; // Return empty AST on failure
    }
    
    // Create a copy of the parser's AST
    AST ast = parser.getAST();
    
    std::cout << "Parse successful, AST constructed" << std::endl;
    std::cout << "Parser output files written to: " << parserOutDir << std::endl;
    return ast;
}

// Phase 3: Symbol Table Generation
void runSymbolTablePhase(AST& ast, const std::string& inputFile, SymbolTableVisitor& symbolTableVisitor) {
    std::cout << "\n=========Phase 3: Symbol Table Generation=========" << std::endl;
    
    // Extract directory and filename
    fs::path inputPath(inputFile);
    fs::path directory = inputPath.parent_path();
    fs::path symtabOutDir;
    
    if (directory.empty()) {
        // If no directory specified, use current directory
        symtabOutDir = "symtab_out";
    } else {
        // Use the same directory as input file
        symtabOutDir = directory / "symtab_out";
    }
    
    // Create symtab_out directory if it doesn't exist
    if (!fs::exists(symtabOutDir)) {
        std::cout << "Creating symbol table output directory: " << symtabOutDir << std::endl;
        fs::create_directories(symtabOutDir);
    }
    
    // Create output path for symbol table files
    fs::path outputBase = symtabOutDir / inputPath.stem();
    std::string outputPath = outputBase.string() + ".outsymboltables";
    
    ASTNode *root = ast.getRoot();
    root->accept(&symbolTableVisitor);
    
    // Output the symbol table to file
    symbolTableVisitor.outputSymbolTable(outputPath);
    std::cout << "Symbol table generated: " << outputPath << std::endl;
}

// Phase 4: Semantic Analysis
bool runSemanticPhase(AST& ast, SymbolTableVisitor& symbolTableVisitor, const std::string& inputFile) {
    std::cout << "\n=========Phase 4: Semantic Analysis=========" << std::endl;
    
    // Extract directory and filename
    fs::path inputPath(inputFile);
    fs::path directory = inputPath.parent_path();
    fs::path semanticsOutDir;
    
    if (directory.empty()) {
        // If no directory specified, use current directory
        semanticsOutDir = "semantics_out";
    } else {
        // Use the same directory as input file
        semanticsOutDir = directory / "semantics_out";
    }
    
    // Create semantics_out directory if it doesn't exist
    if (!fs::exists(semanticsOutDir)) {
        std::cout << "Creating semantics output directory: " << semanticsOutDir << std::endl;
        fs::create_directories(semanticsOutDir);
    }
    
    // Create output path for semantic errors file
    fs::path outputBase = semanticsOutDir / inputPath.stem();
    std::string outputPath = outputBase.string();
    
    SemanticCheckingVisitor semanticChecker(symbolTableVisitor.getGlobalTable());
    semanticChecker.importSymbolTableErrors(symbolTableVisitor);
    ast.getRoot()->accept(&semanticChecker);
    
    // Output semantic errors to file
    semanticChecker.outputErrors(outputPath);
    
    if (semanticChecker.hasErrors()) {
        std::cout << "Semantic errors found. See " << outputPath << ".outsemanticerrors for details." << std::endl;
        return false;
    }
    
    std::cout << "No semantic errors found." << std::endl;
    return true;
}

// Phase 5: Memory Size Allocation
void runMemoryPhase(AST& ast, const std::string& inputFile, MemSizeVisitor& memSizeVisitor) {
    std::cout << "\n=========Phase 5: Memory Size Allocation=========" << std::endl;
    
    // Extract directory and filename
    fs::path inputPath(inputFile);
    fs::path directory = inputPath.parent_path();
    fs::path memsizeOutDir;
    
    if (directory.empty()) {
        // If no directory specified, use current directory
        memsizeOutDir = "memsize_out";
    } else {
        // Use the same directory as input file
        memsizeOutDir = directory / "memsize_out";
    }
    
    // Create memsize_out directory if it doesn't exist
    if (!fs::exists(memsizeOutDir)) {
        std::cout << "Creating memory size output directory: " << memsizeOutDir << std::endl;
        fs::create_directories(memsizeOutDir);
    }
    
    // Create output path for memory size files
    fs::path outputBase = memsizeOutDir / inputPath.stem();
    std::string outputPath = outputBase.string() + ".sizesymboltable";
    
    memSizeVisitor.processAST(ast.getRoot());
    memSizeVisitor.calculateMemorySizes();
    
    // Output the updated symbol table with memory sizes
    memSizeVisitor.outputSymbolTable(outputPath);
    std::cout << "Memory-sized symbol table generated: " << outputPath << std::endl;
}

// Phase 6: Code Generation
bool runCodeGenPhase(AST ast, std::shared_ptr<SymbolTable> symbolTable, const std::string& inputFile) {
    std::cout << "\n=========Phase 6: Code Generation=========" << std::endl;
    
    // Extract directory and filename
    fs::path inputPath(inputFile);
    fs::path directory = inputPath.parent_path();
    fs::path codegenOutDir;
    
    if (directory.empty()) {
        // If no directory specified, use current directory
        codegenOutDir = "codegen_out";
    } else {
        // Use the same directory as input file
        codegenOutDir = directory / "codegen_out";
    }
    
    // Create codegen_out directory if it doesn't exist
    if (!fs::exists(codegenOutDir)) {
        std::cout << "Creating code generation output directory: " << codegenOutDir << std::endl;
        fs::create_directories(codegenOutDir);
    }
    
    // Create output path for MOON code file
    fs::path outputBase = codegenOutDir / inputPath.stem();
    std::string moonCodeFile = outputBase.string() + ".m";
    
    CodeGenVisitor codeGenVisitor(symbolTable);
    codeGenVisitor.processAST(ast.getRoot());
    
    // Output MOON code to file
    codeGenVisitor.generateOutputFile(moonCodeFile);
    std::cout << "MOON machine code generated: " << moonCodeFile << std::endl;
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string tableFile = "attribute_grammar_parsing_table.csv";
    std::string outputDir = ".";
    std::string inputFile;
    CompilerPhase targetPhase = CompilerPhase::CODEGEN; // Default to full compilation

    // Parse command line arguments
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
        } else if (arg == "-p" || arg == "--phase") {
            if (i + 1 < argc) {
                std::string phase = argv[i + 1];
                std::transform(phase.begin(), phase.end(), phase.begin(), 
                              [](unsigned char c){ return std::tolower(c); });
                
                auto it = phaseMap.find(phase);
                if (it != phaseMap.end()) {
                    targetPhase = it->second;
                } else {
                    std::cerr << "Error: Unknown compiler phase: " << phase << "\n";
                    printUsage(argv[0]);
                    return 1;
                }
                i += 2;
            } else {
                std::cerr << "Error: --phase requires an argument.\n";
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
    std::cout << "\n\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;
    std::cout << "Processing file: " << inputFile << std::endl;
    std::cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;

    // Run the compiler phases sequentially
    
    // Phase 1: Lexical Analysis
    Scanner scanner = runScannerPhase(inputFile);
    
    // Stop if only lexical analysis was requested
    if (targetPhase == CompilerPhase::SCAN) {
        std::cout << "Stopping after lexical analysis as requested." << std::endl;
        return 0;
    }

    // Phase 2: Syntax Analysis
    // In main(), change to receive by value
    AST ast = runParserPhase(inputFile, tableFile, scanner);
    
    if (ast.getRoot()==nullptr) {
        return 1; // Error in parsing
    }
    
    // Stop if only syntax analysis was requested
    if (targetPhase == CompilerPhase::PARSE) {
        std::cout << "Stopping after syntax analysis as requested." << std::endl;
        return 0;
    }

    // Phase 3: Symbol Table Generation
    SymbolTableVisitor symbolTableVisitor;
    runSymbolTablePhase(ast, inputFile, symbolTableVisitor);
    
    // Stop if only symbol table generation was requested
    if (targetPhase == CompilerPhase::SYMBOL) {
        std::cout << "Stopping after symbol table generation as requested." << std::endl;
        return 0;
    }

    // Phase 4: Semantic Analysis
    bool semanticSuccess = runSemanticPhase(ast, symbolTableVisitor, inputFile);
    
    // Stop if only semantic analysis was requested
    if (targetPhase == CompilerPhase::SEMANTIC) {
        std::cout << "Stopping after semantic analysis as requested." << std::endl;
        return 0;
    }

    // Continue only if semantic analysis succeeded
    // if (!semanticSuccess) {
    //     std::cout << "Stopping due to semantic errors." << std::endl;
    //     return 1;
    // }

    // Phase 5: Memory Size Allocation
    MemSizeVisitor memSizeVisitor(symbolTableVisitor.getGlobalTable());
    runMemoryPhase(ast, inputFile, memSizeVisitor);
    
    // Stop if only memory allocation was requested
    if (targetPhase == CompilerPhase::MEMORY) {
        std::cout << "Stopping after memory allocation as requested." << std::endl;
        return 0;
    }

    // Phase 6: Code Generation
    bool codeGenSuccess = runCodeGenPhase(ast, memSizeVisitor.getGlobalTable(), inputFile);
    
    if (codeGenSuccess) {
        std::cout << "\nCompilation complete." << std::endl;
    } else {
        std::cout << "Code generation failed." << std::endl;
        return 1;
    }

    std::cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n" << std::endl;
    return 0;
}