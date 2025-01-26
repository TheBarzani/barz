#pragma once
#include "Scanner/NFA.h"
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

class RegexParser {
public:
    static NFA parse(const std::string& regex);

private:
    // Helper classes and methods
    enum TokenType { LITERAL, UNION, STAR, CONCAT, LPAREN, RPAREN };

    struct Token {
        char symbol;
        TokenType type;
    };

    static std::vector<Token> tokenize(const std::string& regex);
    static std::vector<Token> infixToPostfix(const std::vector<Token>& tokens);
    static int precedence(TokenType op);
};