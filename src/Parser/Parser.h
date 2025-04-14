/**
 * @brief Responsible for syntactical parsing of an input file using a parsing table.
 *
 * The Parser class implements the logic to perform syntactic analysis on the tokenized input
 * received from a Scanner. It uses a ParsingTable to drive the parsing process and maintains a
 * parse stack to generate derivations. The class also handles error detection and recovery
 * mechanisms such as skipping errors during parsing. Detailed logging of derivations and syntax
 * errors is supported via output streams.
 *
 * @author TheBarzani
 * @date 2025-02-17
 * 
 */

#ifndef PARSER_H 
#define PARSER_H

#include <string>
#include <fstream>
#include <stack>
#include "ParsingTable.h"
#include "Scanner/Scanner.h"
#include "ASTGenerator/AST.h"

class Parser {  
    private:
        ParsingTable table;                 // Parsing table used to guide the parsing process.
        Scanner scanner;                    // Scanner object to tokenize the input file.
        std::vector<Token> tokens;          // Vector holding the current list of tokens.
        std::ofstream derivationOutput;     // Output stream for logging derivations.
        std::ofstream errorOutput;          // Output stream for logging syntax errors.
        std::stack<std::string> parseStack; // Stack used for holding parsing symbols.
        Token lookahead;                    // The current lookahead token.
        std::string filename;               // Name of the file being parsed.
        std::vector<std::string> derivations;    // List of derivation strings produced during parsing.
        std::vector<std::string> syntaxErrors;   // List of detected syntax errors.
        std::string currentDerivation;      // Current derivation in progress.
        AST ast;                            // Abstract syntax tree (AST) object.

    public:
    /**
     * @brief Initializes the Parser with the given parsing table and input file.
     * @param parsingTable Path to the file containing the parsing table.
     * @param inputFile Path to the source code file to be parsed.
     * @param scanner Scanner object for tokenizing the input.
     */
    Parser(const std::string& parsingTable, const std::string& inputFile, const Scanner& scanner);
    
    /**
     * @brief Initializes the Parser with the given parsing table and input file.
     * @param inputFile Path to the source code file to be parsed.
     * @param parsingTable Path to the file containing the parsing table.
     */
    Parser(const std::string& inputFile, const std::string& parsingTable);

    /**
     * @brief Cleans up any resources used by the Parser.
     */
    ~Parser();
    
    /**
     * @brief Starts the syntactical parsing process.
     * @return true if the parsing completes successfully, false otherwise.
     * 
     * This method drives the main parsing algorithm, consuming tokens from the input,
     * consulting the parsing table, and applying appropriate productions until
     * the input is fully processed or an unrecoverable error is encountered.
     */
    bool parse();
    
    /**
     * @brief Attempts to recover from a parsing error by skipping erroneous tokens.
     * @return true if recovery was successful, false otherwise.
     * 
     * This method implements an error recovery strategy to allow parsing to continue
     * after encountering a syntax error, typically by skipping tokens until a 
     * synchronization point is reached.
     */
    bool skipErrors();
    
    /**
     * @brief Pushes the right-hand side production symbols onto the parse stack in reverse order.
     * @param production The production rule whose symbols will be pushed onto the stack.
     * 
     * Since the parse stack processes elements from top to bottom, this method ensures
     * that symbols are pushed in the correct order for top-down parsing.
     */
    void inverseRHSMultiplePush(const std::string& production);
    
    /**
     * @brief Retrieves the next token from the input.
     * @return The next token from the input stream.
     * 
     * This method advances the scanner and returns the next meaningful token
     * from the input source code.
     */
    Token nextToken();

    /**
     * @brief Returns the Abstract Syntax Tree.
     * @return The Abstract Syntax Tree.
     */
    AST& getAST();

     /**
     * @brief Writes all parser output files to the specified directory
     * @param outputPath The directory path where files should be written
     * @return true if files were written successfully, false otherwise
     */
    bool writeOutputFiles(const std::string& outputPath = "");
};

#endif // PARSER_H