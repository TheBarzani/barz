#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "Scanner.h"

std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeMap = {
        // Keywords
        {TOK_INT, "int"}, {TOK_FLOAT, "float"}, {TOK_VOID, "void"},
        {TOK_CLASS, "class"}, {TOK_WHILE, "while"}, {TOK_IF, "if"},
        {TOK_THEN, "then"}, {TOK_ELSE, "else"}, {TOK_RETURN, "return"},
        {TOK_READ, "read"}, {TOK_WRITE, "write"}, {TOK_PUBLIC, "public"},
        {TOK_PRIVATE, "private"}, {TOK_LOCAL, "local"},
        {TOK_CONSTRUCTOR, "constructor"}, {TOK_ATTRIBUTE, "attribute"},
        {TOK_FUNCTION, "function"}, {TOK_ISA, "isa"},
        {TOK_IMPLEMENTATION, "implementation"}, {TOK_SELF, "self"},

        // Operators/Punctuation
        {TOK_EQ, "eq"}, {TOK_PLUS, "plus"}, {TOK_OR, "or"},
        {TOK_OPENPAR, "openpar"}, {TOK_SEMI, "semi"}, {TOK_NOTEQ, "noteq"},
        {TOK_MINUS, "minus"}, {TOK_AND, "and"}, {TOK_CLOSEPAR, "closepar"},
        {TOK_COMMA, "comma"}, {TOK_LT, "lt"}, {TOK_MULT, "mult"},
        {TOK_NOT, "not"}, {TOK_OPENCUBR, "opencubr"}, {TOK_DOT, "dot"},
        {TOK_GT, "gt"}, {TOK_DIV, "div"}, {TOK_CLOSECUBR, "closecubr"},
        {TOK_COLON, "colon"}, {TOK_LEQ, "leq"}, {TOK_ASSIGN, "assign"},
        {TOK_OPENSQBR, "opensqbr"}, {TOK_ARROW, "arrow"}, {TOK_GEQ, "geq"},
        {TOK_CLOSESQBR, "closesqbr"},

        // Literals/Identifiers
        {TOK_INTNUM, "intnum"}, {TOK_FLOATNUM, "floatnum"}, {TOK_ID, "id"},

        // Comments
        {TOK_INLINE_CMT, "inlinecmt"}, {TOK_BLOCK_CMT, "blockcmt"},

        // Errors
        {TOK_ERR_INVALID_NUM, "Invalidnumber"},
        {TOK_ERR_INVALID_ID, "Invalididentifier"},
        {TOK_ERR_UNKNOWN_CHAR, "Unknownchar"}
    };
    return typeMap.at(type);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.src>\n";
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string inputPath = argv[i];
        std::ifstream inputFile(inputPath);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file: " << inputPath << "\n";
            continue;
        }

        std::string baseName = inputPath.substr(0, inputPath.find_last_of('.'));
        std::ofstream tokenFile(baseName + ".outtextokens");
        std::ofstream errorFile(baseName + ".outlexerrors");

        Scanner scanner(inputFile);
        Token token;

        do {
            token = scanner.getNextToken();
            if (token.type == TOK_SEMI && token.lexeme.empty()) break; // EOF dete

            std::string typeStr = tokenTypeToString(token.type);
            if (typeStr.find("Invalid") != std::string::npos || 
                typeStr == "Unknownchar") {
                errorFile << "[" << typeStr << ":" << token.lexeme 
                          << "] at line " << token.line << "\n";
            } else {
                tokenFile << "[" << typeStr << ", " << token.lexeme 
                          << ", " << token.line << "]\n";
            }
        } while (token.type != TOK_SEMI);

        inputFile.close();
    }
    return 0;
}