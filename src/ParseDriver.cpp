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

 #include <Parser/Parser.h>
 #include <iostream>
 #include <string>
 
 int main(int argc, char* argv[]) {
    
    if (argc > 1 ) {
        Parser parser(argv[1], "parsing_table.csv");
        parser.parse();
    } else {
        std::cout << "Failed to load parsing table" << std::endl;
    }

    return 0;
 }