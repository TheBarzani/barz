#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

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

public:
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
            return ""; // Non-terminal not found
        }

        auto termIt = ntIt->second.find(terminal);
        if (termIt == ntIt->second.end()) {
            return ""; // Terminal not found for this non-terminal
        }

        return termIt->second;
    }

    // Helper methods to access table information
    const std::vector<std::string>& getTerminals() const { return terminals; }
    const std::vector<std::string>& getNonTerminals() const { return nonTerminals; }

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
};