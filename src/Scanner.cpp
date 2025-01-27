#include "Scanner.h"
#include <cctype>
#include <algorithm>

Scanner::Scanner(std::ifstream& input) : input(input) {
    keywords = {
        {"int", TOK_INT}, {"float", TOK_FLOAT}, {"void", TOK_VOID},
        {"class", TOK_CLASS}, {"while", TOK_WHILE}, {"if", TOK_IF},
        {"then", TOK_THEN}, {"else", TOK_ELSE}, {"return", TOK_RETURN},
        {"read", TOK_READ}, {"write", TOK_WRITE}, {"public", TOK_PUBLIC},
        {"private", TOK_PRIVATE}, {"local", TOK_LOCAL},
        {"constructor", TOK_CONSTRUCTOR}, {"attribute", TOK_ATTRIBUTE},
        {"function", TOK_FUNCTION}, {"isa", TOK_ISA},
        {"implementation", TOK_IMPLEMENTATION}, {"self", TOK_SELF}
    };
    advance();
}

void Scanner::advance() {
    if (input.get(currentChar)) {
        if (currentChar == '\n') line++;
    } else {
        currentChar = EOF;
    }
}

void Scanner::skipWhitespace() {
    while (isspace(currentChar) && currentChar != '\n') advance();
}

Token Scanner::getNextToken() {
    while (true) {
        if (currentChar == EOF) return {TOK_SEMI, "", line};

        if (isspace(currentChar)) {
            skipWhitespace();
            continue;
        }

        if (isalpha(currentChar) || currentChar == '_') {
            return parseIdentifierOrKeyword();
        }

        if (isdigit(currentChar)) {
            return parseNumber();
        }

        if (ispunct(currentChar)) {
            return parseOperatorOrPunctuation();
        }

        Token error = {TOK_ERR_UNKNOWN_CHAR, std::string(1, currentChar), line};
        advance();
        return error;
    }
}

Token Scanner::parseIdentifierOrKeyword() {
    std::string lexeme;
    int startLine = line;

    while (isalnum(currentChar) || currentChar == '_') {
        lexeme += currentChar;
        advance();
    }

    if (keywords.find(lexeme) != keywords.end()) {
        return {keywords[lexeme], lexeme, startLine};
    }

    if (!isalpha(lexeme[0])) {
        return {TOK_ERR_INVALID_ID, lexeme, startLine};
    }

    return {TOK_ID, lexeme, startLine};
}

Token Scanner::parseNumber() {
    std::string lexeme;
    int startLine = line;
    bool isFloat = false;
    bool error = false;

    // Check leading zero
    if (currentChar == '0') {
        lexeme += currentChar;
        advance();
        if (isdigit(currentChar)) error = true; // Leading zero
    }

    while (isdigit(currentChar)) {
        lexeme += currentChar;
        advance();
    }

    // Fraction part
    if (currentChar == '.') {
        isFloat = true;
        lexeme += currentChar;
        advance();

        bool hasDigit = false;
        while (isdigit(currentChar)) {
            lexeme += currentChar;
            hasDigit = true;
            advance();
        }
        if (!hasDigit) error = true;
    }

    // Exponent part
    if (currentChar == 'e' || currentChar == 'E') {
        isFloat = true;
        lexeme += currentChar;
        advance();

        if (currentChar == '+' || currentChar == '-') {
            lexeme += currentChar;
            advance();
        }

        bool hasDigit = false;
        while (isdigit(currentChar)) {
            lexeme += currentChar;
            hasDigit = true;
            advance();
        }
        if (!hasDigit) error = true;
    }

    if (error) return {TOK_ERR_INVALID_NUM, lexeme, startLine};
    return {isFloat ? TOK_FLOATNUM : TOK_INTNUM, lexeme, startLine};
}

Token Scanner::parseOperatorOrPunctuation() {
    std::string lexeme(1, currentChar);
    int startLine = line;

    // Check for multi-character operators
    char nextChar = input.peek();
    std::string potentialOp = lexeme + nextChar;

    if (multiCharOps.find(potentialOp) != multiCharOps.end()) {
        advance();
        lexeme += currentChar;
        advance();
    } else {
        advance();
    }

    // Map to TokenType
    if (lexeme == "==") return {TOK_EQ, lexeme, startLine};
    if (lexeme == "<>") return {TOK_NOTEQ, lexeme, startLine};
    if (lexeme == "<=") return {TOK_LEQ, lexeme, startLine};
    if (lexeme == ">=") return {TOK_GEQ, lexeme, startLine};
    if (lexeme == ":=") return {TOK_ASSIGN, lexeme, startLine};
    if (lexeme == "=>") return {TOK_ARROW, lexeme, startLine};
    if (lexeme == "/*" || lexeme == "//") {
        return handleComment(lexeme == "/*");
    }

    // Single-character operators
    if (lexeme == "+") return {TOK_PLUS, lexeme, startLine};
    if (lexeme == "-") return {TOK_MINUS, lexeme, startLine};
    if (lexeme == "*") return {TOK_MULT, lexeme, startLine};
    if (lexeme == "/") return {TOK_DIV, lexeme, startLine};
    if (lexeme == "(") return {TOK_OPENPAR, lexeme, startLine};
    if (lexeme == ")") return {TOK_CLOSEPAR, lexeme, startLine};
    // ... Add all other single-character mappings

    return {TOK_ERR_UNKNOWN_CHAR, lexeme, startLine};
}

Token Scanner::handleComment(bool isBlock) {
    std::string comment;
    int startLine = line;
    comment += isBlock ? "/*" : "//";

    if (isBlock) {
        // Consume the initial '/' and '*' (already part of lexeme)
        advance(); // Move past '*'
        while (true) {
            if (currentChar == EOF) break;
            comment += currentChar;
            if (currentChar == '*' && input.peek() == '/') {
                comment += '/';
                advance(); // Consume '*'
                advance(); // Consume '/'
                break;
            }
            advance();
        }
    } else {
        while (currentChar != '\n' && currentChar != EOF) {
            comment += currentChar;
            advance();
        }
    }

    return {isBlock ? TOK_BLOCK_CMT : TOK_INLINE_CMT, comment, startLine};
}