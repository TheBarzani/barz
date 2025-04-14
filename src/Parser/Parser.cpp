#include "Parser.h"
#include <stack>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Parser::Parser(const std::string& inputFile, const std::string& parsingTable, const Scanner& scanner)
    : table(ParsingTable(parsingTable)), scanner(scanner), currentDerivation("START") {
    tokens = this->scanner.getTokens();
    filename = inputFile.substr(0, inputFile.size() - 4);
}

Parser::Parser(const std::string& inputFile, const std::string& parsingTable)
    : Parser(inputFile, parsingTable, Scanner(inputFile)) {
}

Parser::~Parser() {
    // Clean memory
    tokens.clear();
    derivations.clear();
    syntaxErrors.clear();
    
}
bool Parser::writeOutputFiles(const std::string& outputPath) {
    std::string basePath = outputPath.empty() ? filename : outputPath;
    bool success = true;

    try {
        // Write AST to file
        ast.writeToFile(basePath + ".dot");
        
        // Write derivations to file
        derivationOutput.open(basePath + ".outderivation");
        if (!derivationOutput.is_open()) {
            std::cerr << "Failed to open derivation output file" << std::endl;
            success = false;
        } else {
            for (const auto& derivation : derivations) {
                derivationOutput << derivation << std::endl;
            }
            derivationOutput.close();
        }

        // Write syntax errors to file
        errorOutput.open(basePath + ".outsyntaxerrors");
        if (!errorOutput.is_open()) {
            std::cerr << "Failed to open syntax errors output file" << std::endl;
            success = false;
        } else {
            for (const auto& error : syntaxErrors) {
                errorOutput << error << std::endl;
            }
            errorOutput.close();
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Exception while writing parser output files: " << e.what() << std::endl;
        return false;
    }
}

bool Parser::parse() {
    parseStack.push("$");
    parseStack.push("START");

    lookahead = nextToken();
    std::string currentLexeme;
    bool error = false;

    while (parseStack.top() != "$") {
        std::string x = parseStack.top();
        // std::cout << "Current token: " << lookahead.type << ", Stack top: " << x << std::endl;

        if (table.isTerminal(x)) {
            if (x == lookahead.type) {
                // std::cout << "Matched terminal: " << x << std::endl;
                parseStack.pop();
                currentLexeme = lookahead.lexeme;
                lookahead = nextToken();
            } else {
                std::cout << "Error: Expected " << x << " but got " << lookahead.type << std::endl;
                syntaxErrors.push_back("Error: Expected " + x + " but got " + lookahead.type);
                skipErrors();
                error = true;
            }
        } else if (table.isNonTerminal(x)) {    
            std::string production = table.getProduction(x, lookahead.type);
            if (production != "error") {
                // std::cout << "Using production: " << production << std::endl;
                // Update the full derivation string:
                std::string oldDerivation = currentDerivation;
                // Replace the first occurrence of non-terminal x with the right-hand side production.
                size_t pos = currentDerivation.find(x);
                if (pos != std::string::npos) {
                    std::istringstream iss(production);
                    std::string temp;
                    // Skip the first two tokens
                    iss >> temp; // Skip the nonterminal
                    iss >> temp; // Skip the arrow (->)

                    std::string remainingProduction = "";
                    // Remove any strings that start with '_' from the production
                    while (iss >> temp) {
                        if (temp.empty() || temp[0] != '_') {
                            remainingProduction += " " + temp;
                        }
                    }
                    // If "&epsilon" is encountered, treat it as an empty string.
                    if (remainingProduction.find("&epsilon") != std::string::npos) {
                        remainingProduction = "";
                    }
                    currentDerivation.replace(pos, x.length(), remainingProduction);
                }
                derivations.push_back("(" + oldDerivation + ", " + production + ")");
                // ---------------------------------------------------------

                parseStack.pop(); // Pop before pushing new symbols
                inverseRHSMultiplePush(production);
            } else {
                std::cout << "No production found for " << x << " with lookahead " << lookahead.type << std::endl;
                syntaxErrors.push_back("No production found for " + x + " with lookahead " + lookahead.type);
                skipErrors();
                error = true;
            }
        }
        else {
            // Perform action on the AST for the corresponding semantic attribute rule.
            ast.performAction(x, currentLexeme, lookahead.line);
            parseStack.pop();
        }
    }

    // Add the final derivation entry if we ended on '$'
    if (parseStack.top() == "$") {
        derivations.push_back("(" + currentDerivation + ", $)");
    }

    // Always return true if the parse reaches "$", even if errors were encountered.
    return (lookahead.type == "$" && !error);
}

Token Parser::nextToken() {
    static auto it = tokens.begin();
    while (it != tokens.end()) {
        Token token = *it++;
        if (token.type != "blockcmt" && token.type != "inlinecmt" &&
            token.type.substr(0, 7) != "invalid") {
            return token;
        }
    }
    return {"$", "", scanner.getLineCount(), scanner.getLineCount()};
}

void Parser::inverseRHSMultiplePush(const std::string& production) {
    if (production.empty()) {
        return;  // Handle empty production (epsilon)
    }

    std::istringstream iss(production);
    std::vector<std::string> symbols;
    std::string symbol;

    // Skip the first two tokens (production format assumed to be: nonterminal → RHS)
    int skippedCount = 0;
    while (skippedCount < 2 && (iss >> symbol)) {
        skippedCount++;
    }

    // Read the remaining symbols to be pushed
    while (iss >> symbol) {
        // Skip "&epsilon" symbols.
        if (symbol == "&epsilon")
            continue;
        symbols.push_back(symbol);
    }

    // Push symbols in reverse order onto the parse stack.
    for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) {
        parseStack.push(*it);
    }
}

bool Parser::skipErrors() {
    std::string A = parseStack.top();
    
    // Determine the expected tokens
    std::string expectedTokens = "";
    if (table.isTerminal(A)) {
        // If top of stack is terminal, we expected exactly that terminal
        expectedTokens = A;
    } else if (table.isNonTerminal(A)) {
        // If top of stack is non-terminal, we expected any token in FIRST(A)
        std::vector<std::string> firstSet = table.getFirstSet(A);
        if (!firstSet.empty()) {
            expectedTokens = "one of [";
            for (size_t i = 0; i < firstSet.size(); i++) {
                expectedTokens += firstSet[i];
                if (i < firstSet.size() - 1) {
                    expectedTokens += ", ";
                }
            }
            expectedTokens += "]";
        }
    }
    
    // Build the error message with expected token information
    std::string errorMsg = "Syntax error at line " + std::to_string(lookahead.line) +
                         ": Unexpected token '" + lookahead.type + "', expected " + 
                         (expectedTokens.empty() ? "different token" : expectedTokens);
    
    std::cerr << errorMsg << std::endl;
    syntaxErrors.push_back(errorMsg);

    // If lookahead is "$" or in FOLLOW(A), recover by popping A.
    if (lookahead.type == "$" || table.isInFollow(A, lookahead.type)) {
        std::cout << "Panic mode recovery: Pop " << A << std::endl;
        parseStack.pop();
        return true;
    }
    
    // If the current token is already acceptable, do nothing.
    if (table.isInFirst(A, lookahead.type) ||
       (table.hasEpsilon(A) && table.isInFollow(A, lookahead.type))) {
        return true;
    }
    
    // Otherwise, skip tokens until we find one in FIRST(A) (or, for an ε‑producing A, in FOLLOW(A)).
    while (lookahead.type != "$" &&
           !table.isInFirst(A, lookahead.type) &&
           !(table.hasEpsilon(A) && table.isInFollow(A, lookahead.type))) {
        std::cout << "Skipping token: " << lookahead.type << std::endl;
        syntaxErrors.push_back("Skipping token: " + lookahead.type);
        lookahead = nextToken();
        // Check immediately—if the new token is acceptable, break out.
        if (table.isInFirst(A, lookahead.type) ||
           (table.hasEpsilon(A) && table.isInFollow(A, lookahead.type))) {
            std::cout << "Found acceptable token: " << lookahead.type << std::endl;
            break;
        }
    }
    return true;
}

AST& Parser::getAST(){
    return ast;
}
