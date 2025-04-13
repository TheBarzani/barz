# BARZ
Broken Automated Reliable Zipper (BARZ): A bare-bone compiler written in CPP.

# TODO

### Necessary
- [x] Fix the code for the semantic analysis where the line number and the error messages are correctly outputted
- [ ] Remove unnecessary semantic actions from the attribute grammer
- [x] Optimize the AST tree and eliminate unnecessary nodes with fixes to the node output.
- [ ] Fix the output directory of the generated files during each phase and the date versioning or overwrite options. Also bring the the outputting function calls to the drivers
- [ ] Fix the error outputing format of the syntax analysis
- [ ] Add unit tests for each phase using the gTest framework and fix the testing files structure
- [ ] Use regex expressions for the lexical analysis part where the regex library is used
- [ ] Generate a DFA systematically for the lexical analyzer
- [ ] Fix the driver code to process multiple files


### Optional
- [ ] Add and fix comments for Doxygen documentation generation
- [ ] Transform the Cmake configuration by adding a cmakelist to each package
- [ ] Json-formatted derivation style for the syntax analysis step
- [ ] Nicer formatted symbol table output
- [ ] Implement Travis CI for CI/CD
- [ ] Fix and format the readMe files
- [ ] Look into package management
- [ ] Check if package management/manager is needed


# Project Technology Stack
- **Programming Language:** C++
- **Build System:** CMake
- **Package Manager:** Conan
- **Testing Framework:** Gtest
- **Code Documentation:** Doxygen
- **Continuous Integration:** Travis CI

# Project Structure
```
barz/@main
├── bin
│   ├── astdriver
│   ├── attribute_grammar_parsing_table.csv
│   ├── lexdriver
│   ├── LL1_parsing_table.csv
│   ├── parsedriver
│   └── semanticanalyzerdriver
├── CMakeLists.txt
├── data
│   ├── ast_generation
│   │   ├── attribute_grammar_alternative.grm
│   │   ├── attribute_grammar_parsing_table.csv
│   │   ├── attribute_grammar.ucalgary.grm
│   │   ├── LL1_grammar_alternative.grm
│   │   └── source_files
│   │       ├── bubblesort.dot
│   │       ├── bubblesort.outderivation
│   │       ├── bubblesort.outlexerrors
│   │       ├── bubblesort.outlextokens
│   │       ├── bubblesort.outsyntaxerrors
│   │       ├── bubblesort.src
│   │       ├── bubblesort.src.FLACI
│   │       ├── polynomial.dot
│   │       ├── polynomial.outderivation
│   │       ├── polynomial.outlexerrors
│   │       ├── polynomial.outlextokens
│   │       ├── polynomial.outsyntaxerrors
│   │       ├── polynomial.src
│   │       └── polynomial.src.FLACI
│   ├── lexical_analysis
│   │   ├── lexnegativegrading.alternative1.outlexerrors
│   │   ├── lexnegativegrading.alternative1.outlextokens
│   │   ├── lexnegativegrading.alternative2.outlexerrors
│   │   ├── lexnegativegrading.alternative2.outlextokens
│   │   ├── lexnegativegrading.src
│   │   ├── lexpositivegrading.sample.outlextokens
│   │   └── lexpositivegrading.src
│   └── syntactical_analysis
│       ├── BNF_grammer.grm
│       ├── first_and_follow_sets_new.csv
│       ├── first_follow_sets_table.csv
│       ├── first_follow_sets_table.html
│       ├── LL1_grammar.ucalgary.grm
│       ├── LL1_parsing_table.csv
│       ├── LL1_parsing_table_mine.csv
│       ├── others
│       │   ├── COMP442.grammar.BNF.clean.manual.grm
│       │   ├── COMP442.grammar.clean.atocc.grm
│       │   ├── COMP442.grammar.clean.ucalgary.grm
│       │   ├── COMP442.grammar.clean.ucalgary.grm.ucalgary
│       │   ├── COMP442.grammar.expressions.atocc.grm
│       │   ├── COMP442.grammar.rm.leftrec.grm
│       │   ├── COMP442.grammar.rm.opt.grm
│       │   └── COMP442.grammar.rm.rept0.grm
│       └── parsing_table_ucalgary.html
├── docs
│   ├── ast_generation_specifications.pdf
│   ├── compiler_design_A1_lexical_analysis_report.pdf
│   ├── compiler_design_A2_syntax_analysis_report.pdf
│   ├── compiler_design_A3_ast_generation_report.pdf
│   ├── compiler_design_A4_semantic_analysis_report.pdf
│   ├── diagram.png
│   ├── lexer_fsa.mermaid
│   ├── lexical_analyzer_specifications.pdf
│   ├── README.md
│   ├── symbol_table_and_semantic_analysis_specifications.pdf
│   └── syntax_analyzer_specifications.pdf
├── examples
│   ├── ast_generation
│   │   └── given_examples
│   │       ├── example1.ast.outast
│   │       ├── example1.dot.outast
│   │       ├── example1.source
│   │       ├── example2.ast.outast
│   │       ├── example2.dot.outast
│   │       ├── example2.source
│   │       ├── example3.ast.outast
│   │       ├── example3.dot.outast
│   │       └── example3.source
│   ├── lexical_analysis
│   │   ├── lexnegativegrading.outlexerrors
│   │   ├── lexnegativegrading.outlextokens
│   │   ├── lexnegativegrading.src
│   │   ├── lexpositivegrading.outlexerrors
│   │   ├── lexpositivegrading.outlextokens
│   │   └── lexpositivegrading.src
│   ├── semantic_analysis
│   │   ├── bubblesort.outsymboltables
│   │   ├── bubblesort.src
│   │   ├── polynomial.outsymboltables
│   │   ├── polynomialsemanticerrors.src
│   │   └── polynomial.src
│   └── syntactical_analysis
│       ├── bubblesort.outderivation
│       ├── bubblesort.outlexerrors
│       ├── bubblesort.outlextokens
│       ├── bubblesort.outsyntaxerrors
│       ├── bubblesort.src
│       ├── helloworld.dot
│       ├── helloworld.outderivation
│       ├── helloworld.outlexerrors
│       ├── helloworld.outlextokens
│       ├── helloworld.outsyntaxerrors
│       ├── helloworld.src
│       ├── polynomial.outderivation
│       ├── polynomial.outlexerrors
│       ├── polynomial.outlextokens
│       ├── polynomial.outsyntaxerrors
│       └── polynomial.src
├── README.md
├── src
│   ├── ASTDriver.cpp
│   ├── ASTGenerator
│   │   ├── AST.cpp
│   │   ├── AST.h
│   │   ├── ASTNode.cpp
│   │   └── ASTNode.h
│   ├── LexDriver.cpp
│   ├── ParseDriver.cpp
│   ├── Parser
│   │   ├── Parser.cpp
│   │   ├── Parser.h
│   │   └── ParsingTable.h
│   ├── Scanner
│   │   ├── Scanner.cpp
│   │   └── Scanner.h
│   ├── Semantics
│   │   ├── SemanticCheckingVisitor.cpp
│   │   ├── SemanticCheckingVisitor.h
│   │   ├── SymbolTableVisitor.cpp
│   │   ├── SymbolTableVisitor.h
│   │   └── Visitor.h
│   └── SemanticsDriver.cpp
├── tests
│   ├── CMakeLists.txt
│   ├── CMakeLists.txt.in
│   ├── data
│   │   ├── astgenerator
│   │   │   ├── bubblesort.dot
│   │   │   ├── bubblesort.src
│   │   │   ├── helloworld.dot
│   │   │   ├── helloworld.src
│   │   │   ├── inheritance.dot
│   │   │   ├── inheritance.src
│   │   │   ├── nested.dot
│   │   │   ├── nested.outderivation
│   │   │   ├── nested.outlexerrors
│   │   │   ├── nested.outlextokens
│   │   │   ├── nested.outsyntaxerrors
│   │   │   ├── nested.src
│   │   │   ├── nested.src.dot
│   │   │   ├── polynomial.dot
│   │   │   ├── polynomial.src
│   │   │   ├── simple_main.dot
│   │   │   └── simple_main.src
│   │   ├── parser
│   │   │   ├── array.src
│   │   │   ├── factorial.src
│   │   │   ├── inheritance.dot
│   │   │   ├── inheritance.outderivation
│   │   │   ├── inheritance.outlexerrors
│   │   │   ├── inheritance.outlextokens
│   │   │   ├── inheritance.outsyntaxerrors
│   │   │   ├── inheritance.src
│   │   │   ├── nested.dot
│   │   │   ├── nested.outderivation
│   │   │   ├── nested.outlexerrors
│   │   │   ├── nested.outlextokens
│   │   │   ├── nested.outsyntaxerrors
│   │   │   ├── nested.src
│   │   │   ├── simple_class.src
│   │   │   ├── simple_main.dot
│   │   │   ├── simple_main.outderivation
│   │   │   ├── simple_main.outlexerrors
│   │   │   ├── simple_main.outlextokens
│   │   │   ├── simple_main.outsyntaxerrors
│   │   │   └── simple_main.src
│   │   ├── scanner
│   │   │   ├── invalidnums.src
│   │   │   ├── invalidops.src
│   │   │   ├── morecomments.src
│   │   │   └── validcomments.src
│   │   └── semantics
│   │       ├── test1.dot
│   │       ├── test1.outderivation
│   │       ├── test1.outlexerrors
│   │       ├── test1.outlextokens
│   │       ├── test1.outsyntaxerrors
│   │       ├── test1.src
│   │       ├── test1.src.outsemanticerrors
│   │       ├── test1.src.outsymboltables
│   │       ├── test2.dot
│   │       ├── test2.outderivation
│   │       ├── test2.outlexerrors
│   │       ├── test2.outlextokens
│   │       ├── test2.outsyntaxerrors
│   │       ├── test2.src
│   │       ├── test2.src.outsemanticerrors
│   │       ├── test2.src.outsymboltables
│   │       ├── test.dot
│   │       ├── test.outderivation
│   │       ├── test.outlexerrors
│   │       ├── test.outlextokens
│   │       ├── test.outsyntaxerrors
│   │       ├── test.src
│   │       ├── test.src.outsemanticerrors
│   │       └── test.src.outsymboltables
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
After the build is complete, you can run the executable by navigation to the bin or build folder.

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
   or

      ```bash
      Usage: ./semanticanalyzerdriver [options] file1 [file2 ...]
      Options:
         -t, --table <csv_file>   Specify a custom parsing table CSV file. Default is 'attribute_grammar_parsing_table.csv'.
         -o, --output <dir>       Specify output directory for symbol tables. Default is current directory.
         -h, --help               Show this help message.
      ```

5. **Run the tests**:
   After the build is complete, you can run the tests.

   ```sh
   tests/TestScanner
   ```
