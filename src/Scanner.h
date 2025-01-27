#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

// Token types enum
enum class TokenType {
    ID,
    INTNUM,
    FLOATNUM,
    OPERATOR,
    PUNCTUATION,
    RESERVED_WORD,
    INLINE_COMMENT,
    BLOCK_COMMENT,
    ERROR,
    END_OF_FILE
};

// Token structure
struct Token {
    std::string type;    // The token type as a string (e.g., "id", "intnum")
    std::string lexeme;  // The actual text
    int line;           // Line number
    int endLine;        // End line number (for block comments)
};

class Scanner {
public:
    Scanner(const std::string& filename);
    ~Scanner();
    
    Token getNextToken();
    void processFile();

private:
    std::ifstream input;
    std::string filename;
    int currentLine;
    int currentColumn;
    char currentChar;
    std::ofstream tokenOutput;
    std::ofstream errorOutput;
    std::string currentLineText;
    
    void getNextChar();
    void skipWhitespace();
    std::string scanComment(int& endLine);
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isNonZeroDigit(char c) const;
    
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanOperatorOrPunctuation();
    
    static const std::unordered_map<std::string, std::string> reservedWords;
    static const std::unordered_map<std::string, std::string> operators;
    static const std::unordered_map<std::string, std::string> punctuation;
    
    void reportError(const std::string& message, const std::string& lexeme);
    std::string getCurrentLine();
};

#endif // SCANNER_H