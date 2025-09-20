# BARZ
Broken Automated Reliable Zipper (BARZ): A bare-bone compiler written in C++.

## 📋 Overview

BARZ is a comprehensive compiler implementation that includes lexical analysis, syntax analysis, abstract syntax tree generation, semantic analysis, and code generation targeting the Moon virtual machine.

## ✨ Features

The compiler supports a rich set of features across different compilation phases:

### Compilation Phases
- **Lexical Analysis**: Tokenization using finite state automata
- **Syntax Analysis**: LL(1) parsing with error recovery
- **AST Generation**: Building abstract syntax trees
- **Semantic Analysis**: Symbol table generation and type checking
- **Memory Allocation**: Size calculation for variables and data structures
- **Code Generation**: Generating Moon assembly code

### Language Features Status

| Feature | Status |
|---------|:------:|
| **Memory Allocation** |  |
| Integers and floats | ✅ |
| Arrays of basic types | ✅ |
| Objects | ✅ |
| Arrays of objects | ✅ |
| **Function Handling** |  |
| Function execution and return | ✅ |
| Parameter passing | ✅ |
| Returning values | ✅ |
| Object parameters | ❌ |
| Returning objects/arrays | ❌ |
| Member function calls | ✅ |
| Function Overloading | ❌ |
| **Statements** |  |
| Assignment statements | ✅ |
| Conditional statements | ✅ |
| Loop statements | ✅ |
| I/O statements | ✅ |
| **Data Access** |  |
| Array element access | ✅ |
| Object member access | ✅ |
| Object array element access | ✅ |
| Nested object access | ✅ |
| **Expressions** |  |
| Arithmetic operations (+, -, *, /) | ✅ |
| Logical operations (and, or) | ✅ |
| Array expressions | ✅ |
| Object member expressions | ✅ |
| Object assignment | ❌ |
| **Inheritance** |  |
| Parent member access | ❌ |
| Child-parent interactions | ❌ |
| **Composition** |  |
| Object aggregation | ✅ |
| **Other** |  |
| Floating point operations | ❌ |

## 🛠️ Technology Stack

- **Programming Language:** C++
- **Build System:** CMake
- **Testing Framework:** Google Test (Gtest)
- **Target Platform:** Moon Virtual Machine

## 📁 Project Structure

The project follows a well-organized structure:

```bash
barz/@main
├── CMakeLists.txt
├── README.md
├── data
│   ├── ast_generation
│   │   ├── LL1_grammar_alternative.grm
│   │   ├── attribute_grammar.ucalgary.grm
│   │   ├── attribute_grammar_alternative.grm
│   │   ├── attribute_grammar_parsing_table.csv
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
│       ├── LL1_grammar.ucalgary.grm
│       ├── LL1_parsing_table.csv
│       ├── LL1_parsing_table_mine.csv
│       ├── first_and_follow_sets_new.csv
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
│       └── parsing_table_ucalgary.html
├── docs
│   ├── README.md
│   ├── ast_generation_specifications.pdf
│   ├── code_generation_specifications.pdf
│   ├── compiler_design_A1_lexical_analysis_report.pdf
│   ├── compiler_design_A2_syntax_analysis_report.pdf
│   ├── compiler_design_A3_ast_generation_report.pdf
│   ├── compiler_design_A4_semantic_analysis_report.pdf
│   ├── compiler_design_A5_code_generation_report.pdf
│   ├── diagram.png
│   ├── lexer_fsa.mermaid
│   ├── lexical_analyzer_specifications.pdf
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
│   ├── code_generation
│   │   ├── example-bubblesort.src
│   │   ├── example-polynomial.src
│   │   └── example-simplemain.src
│   ├── lexical_analysis
│   │   ├── lexnegativegrading.outlexerrors
│   │   ├── lexnegativegrading.outlextokens
│   │   ├── lexnegativegrading.src
│   │   ├── lexpositivegrading.outlexerrors
│   │   ├── lexpositivegrading.outlextokens
│   │   └── lexpositivegrading.src
│   ├── semantic_analysis
│   │   ├── bubblesort.dot
│   │   ├── bubblesort.outderivation
│   │   ├── bubblesort.outlexerrors
│   │   ├── bubblesort.outlextokens
│   │   ├── bubblesort.outsymboltables
│   │   ├── bubblesort.outsyntaxerrors
│   │   ├── bubblesort.src
│   │   ├── bubblesort.src.outsymboltables
│   │   ├── polynomial.dot
│   │   ├── polynomial.outderivation
│   │   ├── polynomial.outlexerrors
│   │   ├── polynomial.outlextokens
│   │   ├── polynomial.outsymboltables
│   │   ├── polynomial.outsyntaxerrors
│   │   ├── polynomial.src
│   │   ├── polynomial.src.outsemanticerrors
│   │   ├── polynomial.src.outsymboltables
│   │   ├── polynomialsemanticerrors.dot
│   │   ├── polynomialsemanticerrors.outderivation
│   │   ├── polynomialsemanticerrors.outlexerrors
│   │   ├── polynomialsemanticerrors.outlextokens
│   │   ├── polynomialsemanticerrors.outsyntaxerrors
│   │   ├── polynomialsemanticerrors.src
│   │   ├── polynomialsemanticerrors.src.outsemanticerrors
│   │   └── polynomialsemanticerrors.src.outsymboltables
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
├── moon
│   ├── doc
│   │   ├── moondoc.pdf
│   │   └── moondoc.tex
│   ├── samples
│   │   ├── README
│   │   ├── arithmetics.m
│   │   ├── countchars.m
│   │   ├── factorials.m
│   │   ├── fixedpoint.m
│   │   ├── genrand.m
│   │   ├── lib.m
│   │   ├── newlib.m
│   │   ├── sample.m
│   │   ├── stringops.m
│   │   └── util.m
│   └── source
│       └── moon.c
├── shell.nix
├── src
│   ├── ASTDriver.cpp
│   ├── ASTGenerator
│   │   ├── AST.cpp
│   │   ├── AST.h
│   │   ├── ASTNode.cpp
│   │   └── ASTNode.h
│   ├── CodeGenDriver.cpp
│   ├── CodeGenerator
│   │   ├── CodeGenVisitor.cpp
│   │   ├── CodeGenVisitor.h
│   │   ├── MemSizeVisitor.cpp
│   │   └── MemSizeVisitor.h
│   ├── CompilerDriver.cpp
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
│   ├── TestDriver.cpp
│   └── data
│       ├── astgenerator/
│       ├── codegen/
│       ├── compiler
│       │   ├── bubblesort.src
│       │   ├── codegen_out
│       │   │   ├── bubblesort.m
│       │   │   ├── polynomial.m
│       │   │   ├── simplemain.m
│       │   │   └── test.m
│       │   ├── memsize_out
│       │   │   ├── bubblesort.sizesymboltable
│       │   │   ├── polynomial.sizesymboltable
│       │   │   ├── polynomialsemanticerrors.sizesymboltable
│       │   │   ├── simplemain.sizesymboltable
│       │   │   └── test.sizesymboltable
│       │   ├── parser_out
│       │   │   ├── bubblesort.dot
│       │   │   ├── bubblesort.outderivation
│       │   │   ├── bubblesort.outsyntaxerrors
│       │   │   ├── polynomial.dot
│       │   │   ├── polynomial.outderivation
│       │   │   ├── polynomial.outsyntaxerrors
│       │   │   ├── polynomialsemanticerrors.dot
│       │   │   ├── polynomialsemanticerrors.outderivation
│       │   │   ├── polynomialsemanticerrors.outsyntaxerrors
│       │   │   ├── polynomialsyntaxerrors.dot
│       │   │   ├── polynomialsyntaxerrors.outderivation
│       │   │   ├── polynomialsyntaxerrors.outsyntaxerrors
│       │   │   ├── simplemain.dot
│       │   │   ├── simplemain.outderivation
│       │   │   ├── simplemain.outsyntaxerrors
│       │   │   ├── test.dot
│       │   │   ├── test.outderivation
│       │   │   └── test.outsyntaxerrors
│       │   ├── polynomial.src
│       │   ├── polynomialsemanticerrors.src
│       │   ├── polynomialsyntaxerrors.src
│       │   ├── scanner_out
│       │   │   ├── bubblesort.outlexerrors
│       │   │   ├── bubblesort.outlextokens
│       │   │   ├── polynomial.outlexerrors
│       │   │   ├── polynomial.outlextokens
│       │   │   ├── polynomialsemanticerrors.outlexerrors
│       │   │   ├── polynomialsemanticerrors.outlextokens
│       │   │   ├── polynomialsyntacerrors.outlexerrors
│       │   │   ├── polynomialsyntacerrors.outlextokens
│       │   │   ├── polynomialsyntaxerrors.outlexerrors
│       │   │   ├── polynomialsyntaxerrors.outlextokens
│       │   │   ├── simplemain.outlexerrors
│       │   │   ├── simplemain.outlextokens
│       │   │   ├── test.outlexerrors
│       │   │   └── test.outlextokens
│       │   ├── semantics_out
│       │   │   ├── bubblesort.outsemanticerrors
│       │   │   ├── polynomial.outsemanticerrors
│       │   │   ├── polynomialsemanticerrors.outsemanticerrors
│       │   │   ├── simplemain.outsemanticerrors
│       │   │   └── test.outsemanticerrors
│       │   ├── simplemain.src
│       │   ├── symtab_out
│       │   │   ├── bubblesort.outsymboltables
│       │   │   ├── polynomial.outsymboltables
│       │   │   ├── polynomialsemanticerrors.outsymboltables
│       │   │   ├── simplemain.outsymboltables
│       │   │   └── test.outsymboltables
│       │   └── test.src
│       ├── parser/
│       ├── scanner/
│       └── semantics/
└── tools
    ├── grammartool.jar
    ├── lib.m
    ├── moom
    └── moon

40 directories, 297 files
```

## 🚀 Building and Running

### Prerequisites

- C++ compiler with C++17 support
- CMake 3.10 or higher

### Build Instructions

1. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```

2. Generate build files:
   ```sh
   cmake ..
   ```

3. Build the project:
   ```sh
   cmake --build .
   ```

### Running the Compiler

The project provides several executables for different phases of compilation:

#### Lexical Analysis
```sh
./lexdriver <input_file> <output_filename>
```

#### Syntax Analysis
```sh
./parsedriver [--table parsing_table.csv] file1 [file2 ...]
```

#### AST Generation
```sh
./astdriver [--table attribute_grammar_parsing_table.csv] file1 [file2 ...]
```

#### Semantic Analysis
```sh
./semanticanalyzerdriver [options] file1 [file2 ...]
```

#### Full Compiler
```sh
./compilerdriver [options] file

Options:
  -t, --table <csv_file>   Specify parsing table (default: attribute_grammar_parsing_table.csv)
  -o, --output <dir>       Specify output directory
  -p, --phase <phase>      Compilation phase:
                            scan (1): Lexical analysis only
                            parse (2): Syntax analysis
                            symbol (3): Symbol table generation
                            semantic (4): Semantic analysis
                            memory (5): Memory allocation
                            codegen (6): Code generation (default)
  -h, --help               Show help message
```

## 🔍 Grammar Details

The BARZ language uses a context-free grammar that has been transformed into an LL(1) grammar to facilitate predictive parsing. Several modifications have been made to resolve ambiguities:

- Left factorization of array size definitions
- Elimination of expression ambiguities
- Factorization of identifier nesting
- Resolution of statement ambiguities

## 📝 Development TODOs

### In Progress
- Remove unnecessary semantic actions from the attribute grammar
- Fix error output format for syntax analysis
- Add unit tests using gTest framework
- Use regex for lexical analysis

### Future Improvements
- Add and fix comments for Doxygen documentation
- Improve CMake configuration with package-level CMakeLists
- Implement JSON-formatted derivation output
- Improve symbol table output formatting
- Implement CI/CD with Travis CI
- Improve README and documentation
- Evaluate package management needs

## 📚 Documentation

For more detailed information, see the documentation in the docs directory:

- Lexical analyzer specifications
- Syntax analyzer specifications
- AST generation specifications
- Symbol table and semantic analysis specifications
- Code generation specifications

## 📄 License

MIT License

---