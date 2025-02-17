#ifndef PARSER_H 
#define PARSER_H

#include <string>
#include <fstream>
#include <stack>
#include "ParsingTable.h"
#include "Scanner/Scanner.h"

class Parser {  

    public:
        Parser();
        Parser(const std::string& filename);
        ~Parser();
        bool parse(const std::string& input);
        bool loadParsingTable(const std::string& filename);
        bool skipErrors();
        void inverseRHSMultiplePush(std::stack<std::string>& parseStack, const std::string& production);

    private:
        ParsingTable table;
        Scanner scanner;
        std::string currentInput;           ///< current input
        std::string filename;               ///< Name of the input file
        std::ofstream derivationOutput;          ///< Output file stream for tokens
        std::ofstream errorOutput;          ///< Output file stream for errors
        std::stack<std::string> stack;


};

#endif // PARSER_H