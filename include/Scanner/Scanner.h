#pragma once
#include <fstream>
#include <string>
#include <unordered_map>

enum TokenType { /* ... (token types) ... */ };

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

class Lexer {
private:
    std::ifstream input;
    int line = 1, column = 1;
    std::unordered_map<std::string, TokenType> keywords;
    
    void advance();
    Token readNumber();
    std::string readIdentifier();
    
public:
    Lexer(const std::string& filename);
    Token getNextToken();
};