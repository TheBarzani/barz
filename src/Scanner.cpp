#include "Scanner.h"
#include <iostream>
#include <sstream>
#include <cctype>

// Initialize static members with proper token names
const std::unordered_map<std::string, std::string> Scanner::operators = {
    {"==", "eq"}, {"<>", "noteq"}, {"<", "lt"}, {">", "gt"},
    {"<=", "leq"}, {">=", "geq"}, {"+", "plus"}, {"-", "minus"},
    {"*", "mult"}, {"/", "div"}, {":=", "assign"}, {"=>", "arrow"}
};

const std::unordered_map<std::string, std::string> Scanner::punctuation = {
    {"(", "openpar"}, {")", "closepar"}, {"{", "opencubr"}, {"}", "closecubr"},
    {"[", "opensqbr"}, {"]", "closesqbr"}, {",", "comma"}, {".", "dot"},
    {":", "colon"}, {";", "semi"}
};

const std::unordered_map<std::string, std::string> Scanner::reservedWords = {
    {"int", "int"}, {"float", "float"}, {"void", "void"}, {"class", "class"},
    {"self", "self"}, {"isa", "isa"}, {"while", "while"}, {"if", "if"},
    {"then", "then"}, {"else", "else"}, {"read", "read"}, {"write", "write"},
    {"return", "return"}, {"local", "local"}, {"constructor", "constructor"},
    {"attribute", "attribute"}, {"function", "function"}, {"public", "public"},
    {"private", "private"}, {"implementation", "implementation"},
    {"or", "or"}, {"and", "and"}, {"not", "not"}
};

Scanner::Scanner(const std::string& filename) : 
    filename(filename), currentLine(1), currentColumn(0) {
    input.open(filename);
    tokenOutput.open(filename.substr(0, filename.size()-4)  + ".outlextokens");
    errorOutput.open(filename.substr(0, filename.size()-4) + ".outlexerrors");
    getNextChar();
}

Scanner::~Scanner() {
    if (input.is_open()) input.close();
    if (tokenOutput.is_open()) tokenOutput.close();
    if (errorOutput.is_open()) errorOutput.close();
}

void Scanner::getNextChar() {
    currentChar = input.get();
    if (currentChar == '\n') {
        currentLine++;
        currentColumn = 0;
    } else {
        currentColumn++;
    }
}

void Scanner::skipWhitespace() {
    while (isspace(currentChar) && !input.eof()) {
        if (currentChar == '\n') {
            currentLine++;
            currentColumn = 0;
        } else {
            currentColumn++;
        }
        currentChar = input.get();
    }
}

std::string Scanner::scanComment(int& endLine) {
    std::string comment;
    comment += '/';
    
    getNextChar();
    if (currentChar == '/') {  // Inline comment
        do {
            comment += currentChar;
            getNextChar();
        } while (!input.eof() && currentChar != '\n');
        if (currentChar == '\n') {
            comment.pop_back(); // Remove the newline character
        }
        currentLine--;
        endLine = currentLine;
        return comment;
    } 
    else if (currentChar == '*') {  // Block comment
        comment += '*';
        getNextChar();
        int nestingLevel = 1;
        while (nestingLevel > 0 && !input.eof()) {
            if (currentChar == '/') {
                char nextChar = input.peek();
                if (nextChar == '*') {
                    nestingLevel++;
                    comment += currentChar;
                    getNextChar(); // Consume '*'
                    comment += '*';
                    getNextChar();
                } else {
                    comment += currentChar;
                    getNextChar();
                }
            } else if (currentChar == '*') {
                comment += currentChar;
                getNextChar();
                if (currentChar == '/') {
                    nestingLevel--;
                    comment += currentChar;
                    getNextChar(); // Consume '/'
                    if (nestingLevel == 0) break;
                }
            } else {
                if (currentChar == '\n') {
                    comment.pop_back(); // Remove the newline character
                    comment += "\\n";
                } else {
                    comment += currentChar;
                }
                getNextChar();
            }
        }
        endLine = currentLine;
        return comment;
    }
    endLine = currentLine;
    return comment;
}

bool Scanner::isLetter(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Scanner::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Scanner::isNonZeroDigit(char c) const {
    return c >= '1' && c <= '9';
}

Token Scanner::scanIdentifierOrKeyword() {
    std::string lexeme;
    int startLine = currentLine;
    
    while ((isLetter(currentChar) || isDigit(currentChar) || currentChar == '_') && !input.eof()) {
        lexeme += currentChar;
        getNextChar();
    }
    
    auto it = reservedWords.find(lexeme);
    if (it != reservedWords.end()) {
        return {it->second, lexeme, startLine, startLine};
    }
    
    if (isLetter(lexeme[0])) {
        return {"id", lexeme, startLine, startLine};
    }
    
    return {"invalidid", lexeme, startLine, startLine};
}

Token Scanner::scanNumber() {
    std::string lexeme;
    int startLine = currentLine;
    bool isFloat = false;
    std::string fractionPart;
    std::string exponentDigits;
    
    // Integer part
    while (isDigit(currentChar) && !input.eof()) {
        lexeme += currentChar;
        getNextChar();
    }
    
    // Fraction part
    if (currentChar == '.') {
        isFloat = true;
        lexeme += currentChar;
        getNextChar();
        
        while (isDigit(currentChar) && !input.eof()) {
            fractionPart += currentChar;
            lexeme += currentChar;
            getNextChar();
        }
        
    }
    
    if (currentChar == 'e' || currentChar == 'E') {
        isFloat = true;
        lexeme += currentChar;
        getNextChar();
        
        if (currentChar == '+' || currentChar == '-') {
            lexeme += currentChar;
            getNextChar();
        }
        
        while (isDigit(currentChar) && !input.eof()) {
            exponentDigits += currentChar;
            lexeme += currentChar;
            getNextChar();
        }
    }
    
    // Check for trailing letters/underscores
    if (isLetter(currentChar) || currentChar == '_') {
        while (isLetter(currentChar) || isDigit(currentChar) || currentChar == '_') {
            lexeme += currentChar;
            getNextChar();
        }
        return {"invalidnum", lexeme, startLine, startLine};
    }

    if ((lexeme.size() > 1 && lexeme[0] == '0' )|| (exponentDigits.size() > 1 && exponentDigits[0]=='0') || (fractionPart.size() > 1 && fractionPart[fractionPart.size() - 1] == '0')) {
        return {"invalidnum", lexeme, startLine, startLine};
    }
    
    return {isFloat ? "floatnum" : "intnum", lexeme, startLine, startLine};
}

Token Scanner::scanOperatorOrPunctuation() {
    std::string lexeme;
    int startLine = currentLine;
    
    lexeme += currentChar;
    getNextChar();
    
    // Check for two-character operators
    if (!input.eof()) {
        std::string twoChar = lexeme + currentChar;
        auto it = operators.find(twoChar);
        if (it != operators.end()) {
            getNextChar();
            return {it->second, twoChar, startLine, startLine};
        }
    }
    
    // Check single-character operators and punctuation
    auto opIt = operators.find(lexeme);
    if (opIt != operators.end()) {
        return {opIt->second, lexeme, startLine, startLine};
    }
    
    auto punctIt = punctuation.find(lexeme);
    if (punctIt != punctuation.end()) {
        return {punctIt->second, lexeme, startLine, startLine};
    }

    if (lexeme[0] == '_') {
        Token rest = scanIdentifierOrKeyword();
        return {"invalidid", lexeme.append(rest.lexeme) , rest.line, rest.line};
    }

    return {"invalidchar", lexeme, startLine, startLine};
}

Token Scanner::getNextToken() {
    
    skipWhitespace();
    if (input.eof()) {
        return {"eof", "", currentLine, currentLine};
    }
    
    // Handle comments
    if (currentChar == '/') {
        char nextChar = input.peek();
        if (nextChar == '/' || nextChar == '*') {
            int commentStartLine = currentLine; // Capture start line
            int commentEndLine = currentLine; // Capture end line
            std::string comment = scanComment(commentEndLine);
            return {(nextChar == '/') ? "inlinecmt" : "blockcmt", comment, commentStartLine, commentEndLine};
        }
    }
    
    if (isLetter(currentChar)) {
        return scanIdentifierOrKeyword();
    }
    
    if (isDigit(currentChar)) {
        return scanNumber();
    }
    
    return scanOperatorOrPunctuation();
}

void Scanner::processFile() {
    Token token;
    std::string currentOutput;
    int lastLine = -1; // Track the last line number of the output
    
    do {
        token = getNextToken();
        
        if (token.type == "eof") break;
        
        if (token.type.substr(0,7) == "invalid") {
            reportError(token.type, token.lexeme);
        }

        // Format the token output
        std::string tokenStr = "[" + token.type + ", " + token.lexeme + ", " + std::to_string(token.line) + "]";
        
        // Check if the current token's line is greater than the last line + 1
        if (lastLine != -1 && token.line > lastLine + 3) {
            // If there is a gap between the last token's line and the current token's line,
            // output the current line of tokens and add empty lines for the gap.
            tokenOutput << currentOutput << "\n";
            tokenOutput << "\n";
            // Start a new line with the current token.
            currentOutput = tokenStr;
        } else if (!currentOutput.empty() && token.line != lastLine) {
            // If the current token is on a new line, output the current line of tokens.
            tokenOutput << currentOutput << "\n";
            // Start a new line with the current token.
            currentOutput = tokenStr;
        } else {
            // If the current token is on the same line, append it to the current line of tokens.
            if (!currentOutput.empty()) {
            currentOutput += " ";
            }
            currentOutput += tokenStr;
        }
        // Update the last processed line number.
        lastLine = token.endLine;
        
    } while (true);
    
    // Output remaining tokens
    if (!currentOutput.empty()) {
        tokenOutput << currentOutput << "\n";
    }
}

void Scanner::reportError(const std::string& message, const std::string& lexeme) {
    std::string niceMessage;

    if (message == "invalidid") {
        niceMessage = "Invalid identifier";
    } else if (message == "invalidnum") {
        niceMessage = "Invalid number";
    } else if (message == "invalidchar") {
        niceMessage = "Invalid character";
    } else {
        niceMessage = "Unknown error";
    }

    errorOutput << "Lexical error: " << niceMessage << ": \"" << lexeme << "\": line " << currentLine << "." << std::endl;
}