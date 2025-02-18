/**
 * @class Parser
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


class Parser {  
    public:
        // Constructor: Initializes the Parser with the given parsing table and input file.
        Parser(const std::string& parsingTable, const std::string& inputFile);
        
        // Destructor: Cleans up any resources used by the Parser.
        ~Parser();
        
        // parse: Starts the syntactical parsing process.
        bool parse();
        
        // skipErrors: Attempts to recover from a parsing error by skipping erroneous tokens.
        bool skipErrors();
        
        // inverseRHSMultiplePush: Pushes the right-hand side production symbols onto the parse stack in reverse order.
        void inverseRHSMultiplePush(const std::string& production);
        
        // nextToken: Retrieves the next token from the input.
        Token nextToken();

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
};
#endif // PARSER_H