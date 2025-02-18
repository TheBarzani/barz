/**
 * @file ParsingTable.h
 * @brief Defines the ParsingTable class for constructing and managing parsing tables.
 *
 * This file contains the implementation of the ParsingTable class which is responsible
 * for loading a parsing table from a CSV file, computing FIRST and FOLLOW sets, and providing
 * helper methods to query grammar productions.
 *
 * @author TheBarzani
 * @date 2025-02-17
 */

#ifndef PARSINGTABLE_H
#define PARSINGTABLE_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

/**
 * @brief Represents a parsing table used for syntax analysis.
 */
class ParsingTable {
private:
    /// List of terminal symbols.
    std::vector<std::string> terminals;
    
    /// List of non-terminal symbols.
    std::vector<std::string> nonTerminals;
    
    /// Main parsing table mapping non-terminal -> terminal -> production string.
    std::unordered_map<std::string, 
                      std::unordered_map<std::string, std::string>> table;

    /**
     * @brief Splits a CSV line into separate tokens.
     *
     * Handles empty fields and fields containing commas by checking for quotes.
     *
     * @param line The CSV formatted line to split.
     * @return A vector of tokens extracted from the line.
     */
    std::vector<std::string> splitCSVLine(const std::string& line) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
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

    /// Map storing FIRST sets for non-terminals.
    std::map<std::string, std::set<std::string>> firstSets;
    /// Map storing FOLLOW sets for non-terminals.
    std::map<std::string, std::set<std::string>> followSets;

    /**
     * @brief Computes the FIRST sets for all non-terminals.
     *
     * Iterates over productions until no more changes occur and updates the FIRST sets.
     */
    void computeFirstSets() {
        // Initialize FIRST sets for each non-terminal.
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
                            // If production starts with terminal, add it to FIRST set.
                            changed |= addToFirst(nt, firstSymbol);
                        } else if (isNonTerminal(firstSymbol)) {
                            // If production starts with non-terminal, add its FIRST set.
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
    
    /**
     * @brief Computes the FOLLOW sets for all non-terminals.
     *
     * Initializes the FOLLOW sets and iteratively updates them until stable.
     */
    void computeFollowSets() {
        // Initialize FOLLOW sets for each non-terminal.
        for (const auto& nt : nonTerminals) {
            followSets[nt] = std::set<std::string>();
        }
        
        // Add end marker to the START symbol's FOLLOW set.
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
    
                        // Process each symbol in the production.
                        for (size_t i = 0; i < symbols.size(); i++) {
                            if (isNonTerminal(symbols[i])) {
                                // If not the last symbol, add FIRST set of the next symbol.
                                if (i < symbols.size() - 1) {
                                    auto firstOfNext = getFirstOfSymbol(symbols[i + 1]);
                                    for (const auto& f : firstOfNext) {
                                        changed |= addToFollow(symbols[i], f);
                                    }
                                }
                                // If last symbol, or FIRST of next contains ε, add FOLLOW of current non-terminal.
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
    
    /**
     * @brief Retrieves the FIRST set for a given symbol.
     *
     * If the symbol is a terminal, returns a set containing just that terminal.
     * Otherwise, returns the FIRST set computed for the non-terminal.
     *
     * @param symbol The symbol whose FIRST set is required.
     * @return A set of strings representing the FIRST set of the symbol.
     */
    std::set<std::string> getFirstOfSymbol(const std::string& symbol) {
        if (isTerminal(symbol)) {
            return {symbol};
        }
        return firstSets[symbol];
    }
    
    /**
     * @brief Adds a terminal into the FIRST set of a non-terminal.
     *
     * @param nonTerminal The non-terminal whose FIRST set is to be updated.
     * @param terminal The terminal to add.
     * @return True if the FIRST set was modified, false otherwise.
     */
    bool addToFirst(const std::string& nonTerminal, const std::string& terminal) {
        size_t originalSize = firstSets[nonTerminal].size();
        firstSets[nonTerminal].insert(terminal);
        return firstSets[nonTerminal].size() > originalSize;
    }
    
    /**
     * @brief Adds a terminal into the FOLLOW set of a non-terminal.
     *
     * @param nonTerminal The non-terminal whose FOLLOW set is to be updated.
     * @param terminal The terminal to add.
     * @return True if the FOLLOW set was modified, false otherwise.
     */
    bool addToFollow(const std::string& nonTerminal, const std::string& terminal) {
        size_t originalSize = followSets[nonTerminal].size();
        followSets[nonTerminal].insert(terminal);
        return followSets[nonTerminal].size() > originalSize;
    }

public:
    /**
     * @brief Constructs a ParsingTable by loading from a CSV file.
     *
     * Initializes the parsing table, and computes the FIRST and FOLLOW sets.
     *
     * @param filename The CSV file name from which the parsing table is loaded.
     * @throws std::runtime_error If the CSV file cannot be loaded.
     */
    ParsingTable(const std::string& filename = "parsing_table.csv") {
        if (!loadFromCSV(filename)) {
            throw std::runtime_error("Failed to load parsing table from CSV file: " + filename);
        }
        computeFirstSets();
        computeFollowSets();
    }

    /**
     * @brief Loads the parsing table from a CSV file.
     *
     * Parses the header to get terminal symbols and the subsequent rows for productions.
     *
     * @param filename The CSV file to load.
     * @return True if the file was successfully loaded, false otherwise.
     */
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
        // Print the terminals for debugging.
        std::cout << "Terminals: ";
        for (const auto& terminal : terminals) {
            std::cout << terminal << " ";
        }
        std::cout << std::endl;

        // Read each production line.
        std::string line;
        while (std::getline(file, line)) {
            auto tokens = splitCSVLine(line);
            if (tokens.empty()) continue;

            // First token is the non-terminal.
            std::string nonTerminal = tokens[0];
            nonTerminals.push_back(nonTerminal);

            // Process each terminal column and add production.
            for (size_t i = 1; i < tokens.size() && i <= terminals.size(); ++i) {
                if (!tokens[i].empty()) {
                    table[nonTerminal][terminals[i-1]] = tokens[i];
                }
            }
        }

        return true;
    }

    /**
     * @brief Retrieves a production for a given non-terminal and terminal.
     *
     * If no production is found, returns "error".
     *
     * @param nonTerminal The non-terminal for which the production is requested.
     * @param terminal The terminal for which the production is requested.
     * @return A string representing the production or "error" if not found.
     */
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

    /**
     * @brief Retrieves the list of terminal symbols.
     *
     * @return A constant reference to the vector of terminal symbols.
     */
    const std::vector<std::string>& getTerminals() const { return terminals; }
    
    /**
     * @brief Retrieves the list of non-terminal symbols.
     *
     * @return A constant reference to the vector of non-terminal symbols.
     */
    const std::vector<std::string>& getNonTerminals() const { return nonTerminals; }

    /**
     * @brief Checks if a symbol is a terminal.
     *
     * @param symbol The symbol to check.
     * @return True if the symbol is a terminal, false otherwise.
     */
    bool isTerminal(const std::string& symbol) const {
        return std::find(terminals.begin(), terminals.end(), symbol) != terminals.end();
    }

    /**
     * @brief Checks if a symbol is a non-terminal.
     *
     * @param symbol The symbol to check.
     * @return True if the symbol is a non-terminal, false otherwise.
     */
    bool isNonTerminal(const std::string& symbol) const {
        return std::find(nonTerminals.begin(), nonTerminals.end(), symbol) != nonTerminals.end();
    }

    /**
     * @brief Prints the parsing table for debugging purposes.
     */
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

    /**
     * @brief Checks if a terminal is in the FIRST set of a non-terminal.
     *
     * @param nonTerminal The non-terminal.
     * @param terminal The terminal symbol.
     * @return True if terminal is in the FIRST set, false otherwise.
     */
    bool isInFirst(const std::string& nonTerminal, const std::string& terminal) const {
        auto it = firstSets.find(nonTerminal);
        if (it != firstSets.end()) {
            return it->second.find(terminal) != it->second.end();
        }
        return false;
    }
    
    /**
     * @brief Checks if a terminal is in the FOLLOW set of a non-terminal.
     *
     * @param nonTerminal The non-terminal.
     * @param terminal The terminal symbol.
     * @return True if terminal is in the FOLLOW set, false otherwise.
     */
    bool isInFollow(const std::string& nonTerminal, const std::string& terminal) const {
        auto it = followSets.find(nonTerminal);
        if (it != followSets.end()) {
            return it->second.find(terminal) != it->second.end();
        }
        return false;
    }

    /**
     * @brief Prints all productions for a given non-terminal.
     *
     * @param nonTerminal The non-terminal whose productions are printed.
     */
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

    /**
     * @brief Checks if the non-terminal's FIRST set contains the epsilon symbol.
     *
     * @param nonTerminal The non-terminal to check.
     * @return True if epsilon is present in the FIRST set, false otherwise.
     */
    bool hasEpsilon(const std::string& nonTerminal) const {
        auto it = firstSets.find(nonTerminal);
        if (it != firstSets.end()) {
            return it->second.find("&epsilon") != it->second.end();
        }
        return false;
    }
};

#endif // PARSINGTABLE_H