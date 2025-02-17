#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

class ParsingTable {
private:
    // Store the terminals and non-terminals
    std::vector<std::string> terminals;
    std::vector<std::string> nonTerminals;
    
    // Main parsing table: nonTerminal -> terminal -> production
    std::unordered_map<std::string, 
                      std::unordered_map<std::string, std::string>> table;

    // Split a CSV line into tokens
    std::vector<std::string> splitCSVLine(const std::string& line) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        // Handle empty fields and fields with commas
        bool inQuotes = false;
        std::string current;
        
        for (char ch : line) {
            if (ch == ',' && !inQuotes) {
                tokens.push_back(current);
                current.clear();
            }
            else {
                current += ch;
            }
        }

        if (!current.empty() && current.back() == '\r') { // Remove trailing '\r' if present
            current.pop_back();
        }
        
        tokens.push_back(current); // Add the last token 

        return tokens;
    }

    // Add these member variables if not already present
    std::map<std::string, std::set<std::string>> firstSets;
    std::map<std::string, std::set<std::string>> followSets;

    // Add these new private helper methods
    void computeFirstSets() {
        // Initialize FIRST sets
        for (const auto& nt : nonTerminals) {
            firstSets[nt] = std::set<std::string>();
        }
    
        bool changed;
        do {
            changed = false;
            for (const auto& nt : nonTerminals) {
                for (const auto& term : terminals) {
                    std::string prod = getProduction(nt, term);
                    if (prod != "error") {
                        std::istringstream iss(prod);
                        std::string firstSymbol;
                        iss >> firstSymbol;
                        
                        if (isTerminal(firstSymbol)) {
                            // If production starts with terminal, add to FIRST
                            changed |= addToFirst(nt, firstSymbol);
                        } else if (isNonTerminal(firstSymbol)) {
                            // If production starts with non-terminal, add its FIRST set
                            auto firstOfSymbol = getFirstOfSymbol(firstSymbol);
                            for (const auto& symbol : firstOfSymbol) {
                                changed |= addToFirst(nt, symbol);
                            }
                        }
                    }
                }
            }
        } while (changed);
    }
    
    void computeFollowSets() {
        // Initialize FOLLOW sets
        for (const auto& nt : nonTerminals) {
            followSets[nt] = std::set<std::string>();
        }
        
        // Add $ to START symbol's FOLLOW set
        followSets["START"].insert("$");
    
        bool changed;
        do {
            changed = false;
            for (const auto& nt : nonTerminals) {
                for (const auto& term : terminals) {
                    std::string prod = getProduction(nt, term);
                    if (prod != "error") {
                        std::istringstream iss(prod);
                        std::vector<std::string> symbols;
                        std::string symbol;
                        while (iss >> symbol) {
                            symbols.push_back(symbol);
                        }
    
                        // Process each symbol in the production
                        for (size_t i = 0; i < symbols.size(); i++) {
                            if (isNonTerminal(symbols[i])) {
                                // If this is not the last symbol
                                if (i < symbols.size() - 1) {
                                    auto firstOfNext = getFirstOfSymbol(symbols[i + 1]);
                                    for (const auto& f : firstOfNext) {
                                        changed |= addToFollow(symbols[i], f);
                                    }
                                }
                                // If this is the last symbol or FIRST of next contains ε
                                if (i == symbols.size() - 1) {
                                    auto followOfNT = followSets[nt];
                                    for (const auto& f : followOfNT) {
                                        changed |= addToFollow(symbols[i], f);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } while (changed);
    }
    
    std::set<std::string> getFirstOfSymbol(const std::string& symbol) {
        if (isTerminal(symbol)) {
            return {symbol};
        }
        return firstSets[symbol];
    }
    
    bool addToFirst(const std::string& nonTerminal, const std::string& terminal) {
        size_t originalSize = firstSets[nonTerminal].size();
        firstSets[nonTerminal].insert(terminal);
        return firstSets[nonTerminal].size() > originalSize;
    }
    
    bool addToFollow(const std::string& nonTerminal, const std::string& terminal) {
        size_t originalSize = followSets[nonTerminal].size();
        followSets[nonTerminal].insert(terminal);
        return followSets[nonTerminal].size() > originalSize;
    }

public:
    // Constructor that loads the parsing table from a CSV file
    ParsingTable(const std::string& filename = "parsing_table.csv") {
        if (!loadFromCSV(filename)) {
            throw std::runtime_error("Failed to load parsing table from CSV file: " + filename);
        }
        computeFirstSets();
        computeFollowSets();
    }

    bool loadFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // Read header line to get terminals
        std::string header;
        std::getline(file, header);
        terminals = splitCSVLine(header);
        // First column is empty (row headers), so remove it
        terminals.erase(terminals.begin());
        // Print the terminals
        std::cout << "Terminals: ";
        for (const auto& terminal : terminals) {
            std::cout << terminal << " ";
        }
        std::cout << std::endl;

        // Read each line
        std::string line;
        while (std::getline(file, line)) {
            auto tokens = splitCSVLine(line);
            if (tokens.empty()) continue;

            // First token is the non-terminal
            std::string nonTerminal = tokens[0];
            nonTerminals.push_back(nonTerminal);

            // Process each terminal column
            for (size_t i = 1; i < tokens.size() && i <= terminals.size(); ++i) {
                if (!tokens[i].empty()) {
                    table[nonTerminal][terminals[i-1]] = tokens[i];
                }
            }
        }

        return true;
    }

    // Get a production rule for a given non-terminal and terminal
    std::string getProduction(const std::string& nonTerminal, 
                            const std::string& terminal) const {
        auto ntIt = table.find(nonTerminal);
        if (ntIt == table.end()) {
            return "error"; // Non-terminal not found
        }

        auto termIt = ntIt->second.find(terminal);
        if (termIt == ntIt->second.end()) {
            return "error"; // Terminal not found for this non-terminal
        }

        return termIt->second;
    }

    // Helper methods to access table information
    const std::vector<std::string>& getTerminals() const { return terminals; }
    const std::vector<std::string>& getNonTerminals() const { return nonTerminals; }

    // Check if a string is a terminal
    bool isTerminal(const std::string& symbol) const {
        return std::find(terminals.begin(), terminals.end(), symbol) != terminals.end();
    }

    // Check if a string is a non-terminal
    bool isNonTerminal(const std::string& symbol) const {
        return std::find(nonTerminals.begin(), nonTerminals.end(), symbol) != nonTerminals.end();
    }

    // Print the parsing table (for debugging)
    void printTable() const {
        for (const auto& nt : nonTerminals) {
            for (const auto& term : terminals) {
                std::string prod = getProduction(nt, term);
                if (!prod.empty()) {
                    std::cout << "M[" << nt << ", " << term << "] = " << prod << std::endl;
                }
            }
        }
    }

    bool isInFirst(const std::string& nonTerminal, const std::string& terminal) const {
        auto it = firstSets.find(nonTerminal);
        if (it != firstSets.end()) {
            return it->second.find(terminal) != it->second.end();
        }
        return false;
    }
    
    bool isInFollow(const std::string& nonTerminal, const std::string& terminal) const {
        auto it = followSets.find(nonTerminal);
        if (it != followSets.end()) {
            return it->second.find(terminal) != it->second.end();
        }
        return false;
    }

    void printProductions(const std::string& nonTerminal) {
        std::cout << "Productions for " << nonTerminal << ":\n";
        if (table.find(nonTerminal) != table.end()) {
            for (const auto& [terminal, prod] : table[nonTerminal]) {
                std::cout << "  On " << terminal << ": " << prod << "\n";
            }
        } else {
            std::cout << "  No productions found\n";
        }
    }
};