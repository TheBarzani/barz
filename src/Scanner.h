#ifndef SCANNER_H
#define SCANNER_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

enum TokenType {
    // Keywords
    TOK_INT, TOK_FLOAT, TOK_VOID, TOK_CLASS, TOK_WHILE, TOK_IF,
    TOK_THEN, TOK_ELSE, TOK_RETURN, TOK_READ, TOK_WRITE, TOK_PUBLIC,
    TOK_PRIVATE, TOK_LOCAL, TOK_CONSTRUCTOR, TOK_ATTRIBUTE, TOK_FUNCTION,
    TOK_ISA, TOK_IMPLEMENTATION, TOK_SELF,

    // Operators/Punctuation
    TOK_EQ, TOK_PLUS, TOK_OR, TOK_OPENPAR, TOK_SEMI,
    TOK_NOTEQ, TOK_MINUS, TOK_AND, TOK_CLOSEPAR, TOK_COMMA,
    TOK_LT, TOK_MULT, TOK_NOT, TOK_OPENCUBR, TOK_DOT,
    TOK_GT, TOK_DIV, TOK_CLOSECUBR, TOK_COLON, TOK_LEQ,
    TOK_ASSIGN, TOK_OPENSQBR, TOK_ARROW, TOK_GEQ, TOK_CLOSESQBR,

    // Literals/Identifiers
    TOK_INTNUM, TOK_FLOATNUM, TOK_ID,

    // Comments
    TOK_INLINE_CMT, TOK_BLOCK_CMT,

    // Errors
    TOK_ERR_INVALID_NUM, TOK_ERR_INVALID_ID, TOK_ERR_UNKNOWN_CHAR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

class Scanner {
public:
    Scanner(std::ifstream& input);
    Token getNextToken();

private:
    std::ifstream& input;
    int line = 1;
    char currentChar = ' ';
    
    std::unordered_map<std::string, TokenType> keywords;
    std::unordered_set<std::string> multiCharOps = {
        "==", "<>", "<=", ">=", ":=", "=>", "/*", "//", "[]"
    };

    void advance();
    void skipWhitespace();
    Token parseIdentifierOrKeyword();
    Token parseNumber();
    Token parseOperatorOrPunctuation();
    Token handleComment(bool isBlock);
};

#endif