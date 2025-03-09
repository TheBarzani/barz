#include <Parser/Parser.h>
#include <iostream>
#include <string>
#include <vector>

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [--table parsing_table.csv] file1 [file2 ...]\n"
              << "Options:\n"
              << "  --table <csv_file>   Specify a custom parsing table CSV file. Default is 'parsing_table.csv'.\n"
              << "  -h, --help           Show this help message.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string tableFile = "attribute_grammar_parsing_table.csv";
    std::vector<std::string> inputFiles;

    for (int i = 1; i < argc; ) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-t" || arg == "--table") {
            if (i + 1 < argc) {
                tableFile = argv[i + 1];
                i += 2;
            } else {
                std::cerr << "Error: --table requires a CSV file argument.\n";
                return 1;
            }
        } else {
            inputFiles.push_back(arg);
            i++;
        }
    }

    if (inputFiles.empty()) {
        std::cerr << "Error: No input files provided.\n";
        printUsage(argv[0]);
        return 1;
    }

    for (const auto& file : inputFiles) {
        std::cout << "Parsing file: " << file << " with table: " << tableFile << std::endl;
        Parser parser(file, tableFile);
        parser.parse();
    }

    return 0;
}