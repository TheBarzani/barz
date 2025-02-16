/**
 * @file Parse.cpp
 * @brief Entry point for the syntax analysis driver.
 * 
 * This file contains the main function which serves as the entry point for the syntax analysis driver.
 * 
 * Usage:
 * 
 * @author @TheBarzani
 * @date 2025-02-16
 */

 #include <Parser/ParsingTable.h>
 #include <iostream>
 #include <string>
 
 int main(int argc, char* argv[]) {
    ParsingTable parser;
    
    if (argc > 1 && parser.loadFromCSV(argv[1])) {
        // Example: Get specific production
        std::string prod = parser.getProduction("PROG", "class");
        std::cout << "Production for PROG with terminal 'class': " << prod << std::endl;

        // Example: Print all productions
        parser.printTable();
    } else {
        std::cout << "Failed to load parsing table" << std::endl;
    }

    return 0;
 }