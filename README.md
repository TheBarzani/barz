# BARZ
Broken Automated Reliable Zipper (BARZ): A bare-bone compiler written in CPP.

# Project Technology Stack
- **Programming Language:** C++
- **Build System:** CMake
- **Package Manager:** Conan
- **Testing Framework:** Gtest
- **Code Documentation:** Doxygen
- **Continuous Integration:** Travis CI
- **Static Analyzers:** TBD

# Project Structure Blueprint
```
barz/
├── bin
│   ├── lexdriver
│   ├── parsedriver
│   └── parsing_table.csv
├── CMakeLists.txt
├── data
│   ├── lexical_analysis
│   │   ├── lexnegativegrading.alternative1.outlexerrors
│   │   ├── lexnegativegrading.alternative1.outlextokens
│   │   ├── lexnegativegrading.alternative2.outlexerrors
│   │   ├── lexnegativegrading.alternative2.outlextokens
│   │   ├── lexnegativegrading.outlexerrors
│   │   ├── lexnegativegrading.outlextokens
│   │   ├── lexnegativegrading.src
│   │   ├── lexpositivegrading.outlexerrors
│   │   ├── lexpositivegrading.outlextokens
│   │   ├── lexpositivegrading.sample.outlextokens
│   │   └── lexpositivegrading.src
│   └── syntactical_analysis
│       ├── COMP442.grammar.BNF.clean.tool.grm
│       ├── COMP442.grammar.BNF.corrected.grm
│       ├── COMP442.grammar.BNF.original.grm
│       ├── COMP442.grammar.final.grm.ucalgary
│       ├── first_follow_sets_table.csv
│       ├── first_follow_sets_table.html
│       ├── others
│       │   ├── COMP442.grammar.BNF.clean.manual.grm
│       │   ├── COMP442.grammar.clean.atocc.grm
│       │   ├── COMP442.grammar.clean.ucalgary.grm
│       │   ├── COMP442.grammar.clean.ucalgary.grm.ucalgary
│       │   ├── COMP442.grammar.expressions.atocc.grm
│       │   ├── COMP442.grammar.rm.leftrec.grm
│       │   ├── COMP442.grammar.rm.opt.grm
│       │   └── COMP442.grammar.rm.rept0.grm
│       ├── parsing_table_ucalgary.csv
│       ├── parsing_table_ucalgary.html
│       └── source_files
│           ├── bubblesort.outderivation
│           ├── bubblesort.outlexerrors
│           ├── bubblesort.outlextokens
│           ├── bubblesort.outsyntaxerrors
│           ├── bubblesort.src
│           ├── helloworld.outderivation
│           ├── helloworld.outlexerrors
│           ├── helloworld.outlextokens
│           ├── helloworld.outsyntaxerrors
│           ├── helloworld.src
│           ├── polynomial.outderivation
│           ├── polynomial.outlexerrors
│           ├── polynomial.outlextokens
│           ├── polynomial.outsyntaxerrors
│           └── polynomial.src
├── docs
│   ├── compiler_design_A1_lexical_analysis_report.pdf
│   ├── diagram.png
│   ├── lexer_fsa.mermaid
│   ├── lexical_analyzer_specifications.pdf
│   ├── README.md
│   └── syntax_analyzer_specifications.pdf
├── README.md
├── src
│   ├── LexDriver.cpp
│   ├── ParseDriver.cpp
│   ├── Parser
│   │   ├── Parser.cpp
│   │   ├── Parser.h
│   │   ├── parsing_table.csv
│   │   └── ParsingTable.h
│   └── Scanner
│       ├── Scanner.cpp
│       └── Scanner.h
├── tests
│   ├── CMakeLists.txt
│   ├── CMakeLists.txt.in
│   ├── data
│   │   ├── parser
│   │   │   ├── array.src
│   │   │   ├── factorial.src
│   │   │   ├── inheritance.src
│   │   │   ├── nested.src
│   │   │   ├── simple_class.src
│   │   │   └── simple_main.src
│   │   └── scanner
│   │       ├── invalidnums.src
│   │       ├── invalidops.src
│   │       ├── morecomments.src
│   │       └── validcomments.src
│   └── TestScanner.cpp
└── tools
    └── grammartool.jar

```


## Building and Running the Project with CMake (For the Scanner now)

Follow these steps to build and run the `Driver` executable using CMake:

1. **Create a build directory**:
   Open a terminal and navigate to your project directory. Then create a build directory.

   ```sh
   mkdir build
   cd build
   ```

2. **Generate the build files**:
   Run the `cmake` command to generate the build files.

   ```sh
   cmake ..
   ```

3. **Build the project**:
   Use the `cmake --build` command to build the project.

   ```sh
   cmake --build .
   ```

4. **Run the executable**:
   After the build is complete, you can run the executable.

   ```sh
   ./lexdriver <input_file> <output_filename>
   ```

   or 
   ```bash
   Usage: ./parsedriver [--table parsing_table.csv] file1 [file2 ...]
   Options:
      --table <csv_file>   Specify a custom parsing table CSV file. Default is 'parsing_table.csv'.
      -h, --help           Show this help message.
   ```
      or 
   ```bash
   Usage: ./astdriver [--table parsing_table.csv] file1 [file2 ...]
   Options:
      --table <csv_file>   Specify a custom parsing table CSV file. Default is 'attribute_grammar_parsing_table.csv'.
      -h, --help           Show this help message.
   ```
5. **Run the tests**:
   After the build is complete, you can run the tests.

   ```sh
   tests/TestScanner
   ```
