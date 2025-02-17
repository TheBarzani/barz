#include "Parser.h"
#include <stack>
#include <iostream>

Parser::Parser(const std::string& inputFile, const std::string& parsingTable) : table(ParsingTable(parsingTable)) , scanner(Scanner(inputFile)) {
    scanner.processFile();
    tokens = scanner.getTokens();
    scanner.writeOutputsToFile();
}

Parser::~Parser() {
    // Close the output files
    derivationOutput.close();
    errorOutput.close();
}

bool Parser::parse() {
    parseStack.push("$");
    parseStack.push("START"); // Assuming "START" is the start symbol

    // Add this debug code
    std::cout << "Initial parsing table state:\n";
    table.printProductions("START");
    table.printProductions("CLASSDECL");

    lookahead = nextToken();
    bool error = false;

    while (parseStack.top() != "$") {
        std::string x = parseStack.top();
        std::cout << "Current token: " << lookahead.type << ", Stack top: " << x << std::endl; // Add debug output

        if (table.isTerminal(x)) {
            if (x == lookahead.type) {
                std::cout << "Matched terminal: " << x << std::endl; // Add debug output
                parseStack.pop();
                lookahead = nextToken();
            } else {
                std::cout << "Error: Expected " << x << " but got " << lookahead.type << std::endl; // Add debug output
                skipErrors();
                error = true;
            }
        } else {
            std::string production = table.getProduction(x, lookahead.type);
            if (production != "error") {
                std::cout << "Using production: " << production << std::endl; // Add debug output
                parseStack.pop(); // Make sure we pop before pushing new symbols
                inverseRHSMultiplePush(production);
            } else {
                std::cout << "No production found for " << x << " with lookahead " << lookahead.type << std::endl; // Add debug output
                skipErrors();
                error = true;
            }
        }
    }

    return (lookahead.type == "$" && !error);
}

Token Parser::nextToken() {
    static auto it = tokens.begin();
    while (it != tokens.end()) {
        Token token = *it++;
        if (token.type != "blockcmt" && token.type != "inlinecmt" && token.type.substr(0, 7) != "invalid") {
            return token;
        }
    }
    // If exhausted, return an end-of-input token with default positions.
    return {"$", "", -1, -1}; // Use appropriate default values
}

void Parser::inverseRHSMultiplePush(const std::string& production) {
    if (production.empty()) {
        return;  // Handle empty production (epsilon)
    }

    std::istringstream iss(production);
    std::vector<std::string> symbols;
    std::string symbol;

    // Skip the first two symbols
    int skippedCount = 0;
    while (skippedCount < 2 && (iss >> symbol)) {
        skippedCount++;
    }

    // Read the remaining symbols
    while (iss >> symbol) {
        symbols.push_back(symbol);
    }

    // Push symbols in reverse order
    for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) {
        parseStack.push(*it);
        if (table.isTerminal(*it)) {
            std::cout << "Pushed terminal: " << *it << std::endl;
        } else {
            std::cout << "Pushed non-terminal: " << *it << std::endl;
        }
    }
}

bool Parser::skipErrors() {
    std::string currentTop = parseStack.top();
    
    // Report the syntax error with location
    // std::cerr << "Syntax error at line " << lookahead.line << ": Unexpected token '" 
            //   << lookahead.type << "'" << std::endl;
    
    // Case 1: If lookahead is $ or in FOLLOW(top)
    if (lookahead.type == "$" || table.isInFollow(currentTop, lookahead.type)) {
        parseStack.pop();
        return true;
    }
    
    // Case 2: Skip tokens until we find something in FIRST(top) or FOLLOW(top)
    while (!table.isInFirst(currentTop, lookahead.type) && 
           !(table.isInFirst(currentTop, lookahead.type) && 
             table.isInFollow(currentTop, lookahead.type))) {
        
        lookahead = nextToken();
        
        // Check if we've reached the end of input
        if (lookahead.type == "$") {
            return false;
        }
    }
    
    return true;
}

