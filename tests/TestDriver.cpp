#include "Scanner/Scanner.h"
#include <gtest/gtest.h>
#include <fstream>
#include <string>

// Helper function to create a temporary file with given content
std::string createTempFile(const std::string& content) {
    std::string filename = "temp_test_file.src";
    std::ofstream file(filename);
    file << content;
    file.close();
    return filename;
}

// Test scanning identifiers and keywords
TEST(ScannerTest, IdentifiersAndKeywords) {
    std::string content = "int main void class";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);

    Token token = scanner.getNextToken();
    EXPECT_EQ(token.type, "int");
    EXPECT_EQ(token.lexeme, "int");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "id");
    EXPECT_EQ(token.lexeme, "main");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "void");
    EXPECT_EQ(token.lexeme, "void");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "class");
    EXPECT_EQ(token.lexeme, "class");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

// Test scanning numbers
TEST(ScannerTest, Numbers) {
    std::string content = "123 45.67 89e10 3.14e-2";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);

    Token token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "123");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "floatnum");
    EXPECT_EQ(token.lexeme, "45.67");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "89e10");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "floatnum");
    EXPECT_EQ(token.lexeme, "3.14e-2");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

// Test scanning operators and punctuation
TEST(ScannerTest, OperatorsAndPunctuation) {
    std::string content = "+ - * / == <= >= ( ) { }";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);

    Token token = scanner.getNextToken();
    EXPECT_EQ(token.type, "plus");
    EXPECT_EQ(token.lexeme, "+");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "minus");
    EXPECT_EQ(token.lexeme, "-");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "mult");
    EXPECT_EQ(token.lexeme, "*");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "div");
    EXPECT_EQ(token.lexeme, "/");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eq");
    EXPECT_EQ(token.lexeme, "==");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "leq");
    EXPECT_EQ(token.lexeme, "<=");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "geq");
    EXPECT_EQ(token.lexeme, ">=");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "openpar");
    EXPECT_EQ(token.lexeme, "(");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "closepar");
    EXPECT_EQ(token.lexeme, ")");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "opencubr");
    EXPECT_EQ(token.lexeme, "{");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "closecubr");
    EXPECT_EQ(token.lexeme, "}");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

// Test scanning comments
TEST(ScannerTest, Comments) {
    std::string content = "// This is an inline comment\n/* This is a block comment */";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);

    Token token = scanner.getNextToken();
    EXPECT_EQ(token.type, "inlinecmt");
    EXPECT_EQ(token.lexeme, "// This is an inline comment");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "blockcmt");
    EXPECT_EQ(token.lexeme, "/* This is a block comment */");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

// Test scanning invalid tokens
TEST(ScannerTest, InvalidTokens) {
    std::string content = "@ # $";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);

    Token token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "@");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "#");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "$");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

// Test scanning invalid numbers
TEST(ScannerTest, InvalidNumbers) {
    std::string content = R"(
        1.           // Invalid: ends with '.'
        1e           // Invalid: 'e' with no exponent
        123a         // Invalid: non-digit character 'a'
        00           // Invalid: leading zero followed by another digit
        0123         // Invalid: leading zero followed by digits
        1.0e         // Invalid: incomplete exponent
        1e2e3        // Invalid: multiple 'e' characters
        1.5e-        // Invalid: multiple signs in exponent
        123L         // Invalid: non-digit character 'L'
        0x1A         // Invalid: hexadecimal notation (not allowed)
        1_000        // Invalid: underscore in number
    )";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);

    Token token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "1.");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "1e");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "123a");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "00");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "0123");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "1.0e");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "1e2e3");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "1.5e-");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "123L");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "0x1A");

    token = scanner.getNextToken();
    scanner.getNextToken(); // Skip the comment
    EXPECT_EQ(token.type, "invalidnum");
    EXPECT_EQ(token.lexeme, "1_000");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

// Test scanning invalid operators
TEST(ScannerTest, InvalidOperators) {
    std::string content = R"(
        // Invalid operator: #
        a#5;
        // Invalid operator: =
        i = 5;
        // Invalid operator: @
        @5;
        // Invalid operator: %
        10.5 % 2.5;
        // Invalid operator: ^
        7 ^ 2;
        // Invalid operator: &
        4 & 1;
        // Invalid operator: |
        6 | 2;
        // Invalid operator: !
        !3;
    )";
    std::string filename = createTempFile(content);
    Scanner scanner(filename);
    
    scanner.getNextToken(); // Skip the comment
    Token token = scanner.getNextToken();
    EXPECT_EQ(token.type, "id");
    EXPECT_EQ(token.lexeme, "a");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "#");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "5");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "id");
    EXPECT_EQ(token.lexeme, "i");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "=");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "5");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "@");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "5");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "floatnum");
    EXPECT_EQ(token.lexeme, "10.5");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "%");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "floatnum");
    EXPECT_EQ(token.lexeme, "2.5");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "7");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "^");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "2");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "4");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "&");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "1");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "6");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "|");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "2");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    scanner.getNextToken(); // Skip the comment
    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "invalidchar");
    EXPECT_EQ(token.lexeme, "!");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "intnum");
    EXPECT_EQ(token.lexeme, "3");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "semi");
    EXPECT_EQ(token.lexeme, ";");

    token = scanner.getNextToken();
    EXPECT_EQ(token.type, "eof");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}