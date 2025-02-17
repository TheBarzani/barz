

/**
 * @file Scanner.cpp
 * @brief Implementation of the Scanner class for lexical analysis.
 * 
 * This file contains the implementation of the Scanner class, which is responsible for
 * performing lexical analysis on the input source code. It identifies tokens such as
 * operators, punctuation, reserved words, identifiers, numbers, and comments.
 * 
 * @date 2025-01-26
 * @author @TheBarzani
 */

#include "Scanner.h"
#include <iostream>
#include <sstream>
#include <cctype>

/**
 * @brief Initialize static members with proper token names for operators.
 */
const std::unordered_map<std::string, std::string> Scanner::operators = {
    {"==", "eq"}, {"<>", "neq"}, {"<", "lt"}, {">", "gt"},
    {"<=", "leq"}, {">=", "geq"}, {"+", "plus"}, {"-", "minus"},
    {"*", "mult"}, {"/", "div"}, {":=", "assign"}, {"=>", "arrow"}
};

/**
 * @brief Initialize static members with punctuation token names.
 */
const std::unordered_map<std::string, std::string> Scanner::punctuation = {
    {"(", "lpar"}, {")", "rpar"}, {"{", "lcurbr"}, {"}", "rcurbr"},
    {"[", "lsqbr"}, {"]", "rsqbr"}, {",", "comma"}, {".", "dot"},
    {":", "colon"}, {";", "semi"}
};

/**
 * @brief Initialize static members with reserved words token names.
 */
const std::unordered_map<std::string, std::string> Scanner::reservedWords = {
    {"int", "int"}, {"float", "float"}, {"void", "void"}, {"class", "class"},
    {"self", "self"}, {"isa", "isa"}, {"while", "while"}, {"if", "if"},
    {"then", "then"}, {"else", "else"}, {"read", "read"}, {"write", "write"},
    {"return", "return"}, {"local", "local"}, {"constructor", "constructor"},
    {"attribute", "attribute"}, {"function", "function"}, {"public", "public"},
    {"private", "private"}, {"implementation", "implementation"},
    {"or", "or"}, {"and", "and"}, {"not", "not"}
};

/**
 * @brief Constructor with input file only.
 * 
 * @param in The input file name.
 */
Scanner::Scanner(const std::string& in) : Scanner(in,""){};

/**
 * @brief Constructor with input and output files.
 * 
 * @param in The input file name.
 * @param out The output file name.
 */
Scanner::Scanner(const std::string& in, const std::string& out) : 
    filename(in), currentLine(1), currentColumn(0) {
    input.open(filename);
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open file " + filename);
    }
    tokenOutput.open((out == "" ? filename.substr(0, filename.size()-4) : out) + ".outlextokens");
    errorOutput.open((out == "" ? filename.substr(0, filename.size()-4) : out) + ".outlexerrors");
    getNextChar();
}

/**
 * @brief Destructor to close file streams.
 */
Scanner::~Scanner() {
    if (input.is_open()) input.close();
    if (tokenOutput.is_open()) tokenOutput.close();
    if (errorOutput.is_open()) errorOutput.close();
}

/**
 * @brief Get the next character from the input stream.
 */
void Scanner::getNextChar() {
    currentChar = input.get();
    if (currentChar == '\n') {
        currentLine++;
        currentColumn = 0;
    } else {
        currentColumn++;
    }
}

/**
 * @brief Skip whitespace characters in the input stream.
 */
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

/**
 * @brief Scan and return a comment token.
 * 
 * @param endLine Reference to store the end line of the comment.
 * @return std::string The scanned comment.
 */
std::string Scanner::scanComment(int& endLine) {
    std::string comment;
    comment += '/';
    
    getNextChar();
    if (currentChar == '/') {  // Inline comment
        do {
            comment += currentChar;
            getNextChar();
        } while (!input.eof() && currentChar != '\n');
        if (comment[comment.size()-1]=='\n' || comment[comment.size()-1]==13) comment.pop_back(); // Remove the newline character
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

/**
 * @brief Check if a character is a letter.
 * 
 * @param c The character to check.
 * @return true If the character is a letter.
 * @return false Otherwise.
 */
bool Scanner::isLetter(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief Check if a character is a digit.
 * 
 * @param c The character to check.
 * @return true If the character is a digit.
 * @return false Otherwise.
 */
bool Scanner::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

/**
 * @brief Check if a character is a non-zero digit.
 * 
 * @param c The character to check.
 * @return true If the character is a non-zero digit.
 * @return false Otherwise.
 */
bool Scanner::isNonZeroDigit(char c) const {
    return c >= '1' && c <= '9';
}

/**
 * @brief Scan and return an identifier or keyword token.
 * 
 * @return Token The scanned token.
 */
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

/**
 * @brief Scan and return a number token.
 * 
 * @return Token The scanned token.
 */
Token Scanner::scanNumber() {
    std::string lexeme;
    int startLine = currentLine;
    bool isFloat = false;
    bool isExponent = false;
    std::string integerPart;
    std::string fractionPart;
    std::string exponentDigits;
    
    // Integer part
    while (isDigit(currentChar) && !input.eof()) {
        integerPart += currentChar;
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
    
    // Exponent part
    if (currentChar == 'e' || currentChar == 'E') {
        isExponent = true;
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
        return {"invalidlit", lexeme, startLine, startLine};
    }

    // Check for invalid float or exponent without digits
    if ((isFloat && fractionPart.empty()) || (isExponent && exponentDigits.empty())) {
        return {"invalidlit", lexeme, startLine, startLine};
    }

    // Check for invalid number formats
    if ((integerPart.size() > 1 && integerPart[0] == '0' ) || (exponentDigits.size() > 1 && exponentDigits[0]=='0') || (fractionPart.size() > 1 && fractionPart[fractionPart.size() - 1] == '0')) {
        return {"invalidlit", lexeme, startLine, startLine};
    }
    
    return {isFloat ? "floatlit" : "intlit", lexeme, startLine, startLine};
}

/**
 * @brief Scan and return an operator or punctuation token.
 * 
 * @return Token The scanned token.
 */
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

    // Handle invalid identifier starting with underscore
    if (lexeme[0] == '_') {
        Token rest = scanIdentifierOrKeyword();
        return {"invalidid", lexeme.append(rest.lexeme) , rest.line, rest.line};
    }

    return {"invalidchar", lexeme, startLine, startLine};
}

/**
 * @brief Get the next token from the input stream.
 * 
 * @return Token The next token.
 */
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

/**
 * @brief Process the entire input file and generate tokens.
 */
void Scanner::processFile() {
    Token token;
    do {
        token = getNextToken();
        if (token.type == "eof") break;
        tokens.push_back(token);
    } while (true);
}

/**
 * @brief Write the generated tokens to the output file.
 */
void Scanner::writeOutputsToFile() {
    Token token;
    std::string currentOutput;
    int lastLine = -1; // Track the last line number of the output
    
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            
        token = *it;
        
        if (token.type.substr(0,7) == "invalid") {
            reportError(token);
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
        
    }

    // Output remaining tokens
    if (!currentOutput.empty()) {
        tokenOutput << currentOutput << "\n";
    }
    
}

/**
 * @brief Report lexical errors to the error output stream.
 * 
 * @param message The error message.
 * @param lexeme The lexeme that caused the error.
 */
void Scanner::reportError(const Token& token) {
    std::string niceMessage;
    std::string message = token.type;
    if (message == "invalidid") {
        niceMessage = "Invalid identifier";
    } else if (message == "invalidlit") {
        niceMessage = "Invalid literal";
    } else if (message == "invalidchar") {
        niceMessage = "Invalid character";
    } else {
        niceMessage = "Unknown error";
    }

    errorOutput << "Lexical error: " << niceMessage << ": \"" << token.lexeme << "\": line " << token.line << "." << std::endl;
}