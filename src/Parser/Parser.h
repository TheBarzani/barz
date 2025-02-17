#ifndef PARSER_H 
#define PARSER_H

#include <string>
#include <fstream>
#include <stack>
#include "ParsingTable.h"
#include "Scanner/Scanner.h"

class Parser {  

    public:
        Parser(const std::string& parsingTable, const std::string& inputFile);
        ~Parser();
        bool parse();
        bool skipErrors();
        void inverseRHSMultiplePush(const std::string& production);
        Token nextToken();

    private:
        ParsingTable table;                 ///< Parsing table
        Scanner scanner;                    ///< Scanner object
        std::vector<Token> tokens;          ///< current input of tokens
        std::ofstream derivationOutput;     ///< Output file stream for derivations
        std::ofstream errorOutput;          ///< Output file stream for errors
        std::stack<std::string> parseStack; ///< Stack for parsing
        Token lookahead;                    ///< Current lookahead token
};

#endif // PARSER_H