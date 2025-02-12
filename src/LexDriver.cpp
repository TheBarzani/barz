/**
 * @file LexDriver.cpp
 * @brief Entry point for the lexical analysis driver.
 * 
 * This file contains the main function which serves as the entry point for the lexical analysis driver.
 * It initializes the Scanner object with the provided input file and optional output filename, 
 * and processes the file to perform lexical analysis.
 * 
 * Usage:
 * - ./LexDriver <input_file>
 * - ./LexDriver <input_file> <output_filename>
 * 
 * @author @TheBarzani
 * @date 2025-01-26
 */

#include "Scanner/Scanner.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << std::endl;
        std::cout << "- " << argv[0] << " <input_file>" << std::endl;
        std::cout << "- " << argv[0] << " <input_file>" << " <output_filename>"  << std::endl;
        return 1;
    }
    
    try {
        Scanner scanner(argv[1], argc < 3 ? "" : argv[2]);
        scanner.processFile();
        std::cout << "Lexical analysis completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}