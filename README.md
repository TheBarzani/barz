# barz
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
5. **Run the tests**:
   After the build is complete, you can run the tests.

   ```sh
   tests/TestScanner
   ```

## AST Stuff

START -> PROG _createRoot .
PROG -> CLASSIMPLFUNC PROG _addToProgram | .
CLASSIMPLFUNC -> CLASSDECL | IMPLDEF | FUNCDEF .
CLASSDECL -> class id _createClassId ISA1 lcurbr VISMEMBERDECL rcurbr semi _createClass .
VISMEMBERDECL -> VISIBILITY MEMDECL _addMember VISMEMBERDECL | .
ISA1 -> isa id _createInheritanceList _addInheritanceId ISA2 _makeInheritanceList | .
ISA2 -> comma id _addInheritanceId ISA2 _makeInheritanceList | .
IMPLDEF -> implementation id _createImplementationId lcurbr IMPLBODY rcurbr _createImplementation .
IMPLBODY -> FUNCDEF _addImplementationFunction IMPLBODY | .
FUNCDEF -> FUNCHEAD FUNCBODY _createFunction .
VISIBILITY -> public _setVisibility | private _setVisibility .
MEMDECL -> FUNCDECL | ATTRDECL .
FUNCDECL -> FUNCHEAD semi _createFunctionDeclaration .
FUNCHEAD -> function id _createFunctionId lpar FPARAMS rpar arrow RETURNTYPE _createFunctionSignature
| constructor _setConstructor lpar FPARAMS rpar _createConstructorSignature .
FUNCBODY -> lcurbr LOCALVARDECLORSTAT2 rcurbr _createFunctionBody .
LOCALVARDECLORSTAT -> LOCALVARDECL | STATEMENT .
LOCALVARDECLORSTAT2 -> LOCALVARDECLORSTAT _addBodyStatement LOCALVARDECLORSTAT2 | .
ATTRDECL -> attribute VARDECL _createAttribute .
LOCALVARDECL -> local VARDECL _createLocalVariable .
VARDECL -> id _setVariableId colon TYPE _setVariableType ARRAYSIZES semi _createVariable .
STATEMENT -> FUNCALLORASSIGN semi _createExpressionStatement
| if lpar RELEXPR rpar then STATBLOCK else STATBLOCK semi _createIfStatement
| while lpar RELEXPR rpar STATBLOCK semi _createWhileStatement
| read lpar VARIABLE rpar semi _createReadStatement
| write lpar EXPR rpar semi _createWriteStatement
| return lpar EXPR rpar semi _createReturnStatement .
FUNCALLORASSIGN -> IDORSELF _pushIdentifier FUNCALLORASSIGN2 .
FUNCALLORASSIGN2 -> INDICES _processArrayAccess FUNCALLORASSIGN3 | lpar APARAMS rpar _createFunctionCall FUNCALLORASSIGN4 .
FUNCALLORASSIGN3 -> ASSIGNOP EXPR _createAssignment | dot id _processDotAccess FUNCALLORASSIGN2 .
FUNCALLORASSIGN4 -> dot id _processFunctionReturn FUNCALLORASSIGN2 | .
STATBLOCK -> lcurbr STATEMENTS rcurbr _createBlock | STATEMENT _createSingleStatement | .
STATEMENTS -> STATEMENT _addStatement STATEMENTS | .
EXPR -> ARITHEXPR EXPR2 _finishExpression .
EXPR2 -> RELOP ARITHEXPR _createRelationalExpr | .
RELEXPR -> ARITHEXPR RELOP _setRelop ARITHEXPR _createRelationalExpr .
ARITHEXPR -> TERM RIGHTRECARITHEXPR _finishArithExpr .
RIGHTRECARITHEXPR -> ADDOP _pushAddOp TERM _processAddOp RIGHTRECARITHEXPR | .
SIGN -> plus _pushSign | minus _pushSign .
TERM -> FACTOR RIGHTRECTERM _finishTerm .
RIGHTRECTERM -> MULTOP _pushMultOp FACTOR _processMultOp RIGHTRECTERM | .
FACTOR -> IDORSELF _pushIdentifier FACTOR2 REPTVARIABLEORFUNCTIONCALL _finishFactor
| floatlit _pushFloatLiteral
| intlit _pushIntLiteral
| lpar ARITHEXPR rpar _handleParenExpr
| not FACTOR _createNotExpr
| SIGN FACTOR _applySign .
FACTOR2 -> lpar APARAMS rpar _createFunctionCall | INDICES _processArrayAccess .
INDICES -> INDICE _addIndex INDICES | .
REPTVARIABLEORFUNCTIONCALL -> IDNEST _processDotAccess REPTVARIABLEORFUNCTIONCALL | .
VARIABLE -> IDORSELF _pushIdentifier VARIABLE2 _finishVariable .
VARIABLE2 -> INDICES _processArrayAccess REPTVARIABLE | lpar APARAMS rpar _createFunctionCall VARIDNEST .
REPTVARIABLE -> VARIDNEST _processDotAccess REPTVARIABLE | .
VARIDNEST -> dot id _pushDotIdentifier VARIDNEST2 .
VARIDNEST2 -> lpar APARAMS rpar _createFunctionCall VARIDNEST | INDICES _processArrayAccess .
INDICE -> lsqbr ARITHEXPR rsqbr _createArrayIndex .
IDNEST -> dot id _pushDotIdentifier IDNEST2 .
IDNEST2 -> lpar APARAMS rpar _createFunctionCall | INDICES _processArrayAccess .
ARRAYSIZE -> lsqbr ARRAYSIZE2 .
ARRAYSIZE2 -> intlit _addArrayDimension rsqbr | rsqbr _addDynamicArrayDimension .
ARRAYSIZES -> ARRAYSIZE _processArraySize ARRAYSIZES | .
TYPE -> int _setTypeInt | float _setTypeFloat | id _setTypeCustom .
RETURNTYPE -> TYPE | void _setTypeVoid .
APARAMS -> EXPR _addActualParam REPTAPARAMS1 | .
REPTAPARAMS1 -> APARAMSTAIL _continueParamList REPTAPARAMS1 | .
APARAMSTAIL -> comma EXPR _addActualParam .
FPARAMS -> id _setParamId colon TYPE _setParamType ARRAYSIZES _createFormalParam REPTFPARAMS1 | .
REPTFPARAMS1 -> FPARAMSTAIL _addFormalParam REPTFPARAMS1 | .
FPARAMSTAIL -> comma id _setParamId colon TYPE _setParamType ARRAYSIZES _createFormalParam .
ASSIGNOP -> assign _setAssignOperator .
RELOP -> eq _setRelop | neq _setRelop | lt _setRelop | gt _setRelop | lteq _setRelop | gteq _setRelop .
ADDOP -> plus _setAddOp | minus _setAddOp | or _setAddOp .
MULTOP -> mult _setMultOp | div _setMultOp | and _setMultOp .
IDORSELF -> id _setIdentifier | self _setSelfIdentifier .
```

## Description of Actions

### Program Structure Actions
- `_createRoot`: Creates the root PROGRAM node with CLASS_LIST, FUNCTION_LIST, and IMPLEMENTATION_LIST children
- `_addToProgram`: Adds a class, implementation, or function to its appropriate list in the program structure

### Class-Related Actions
- `_createClassId`: Creates a CLASS_ID node with the identifier name
- `_createInheritanceList`: Creates an INHERITANCE_LIST node for parent classes
- `_addInheritanceId`: Creates an INHERITANCE_ID node for a parent class name
- `_makeInheritanceList`: Processes inheritance identifiers into a complete inheritance list
- `_createClass`: Creates a CLASS node with its ID, inheritance list, and member declarations

### Implementation-Related Actions
- `_createImplementationId`: Creates a node for the class being implemented
- `_addImplementationFunction`: Adds a function to an implementation
- `_createImplementation`: Creates an IMPLEMENTATION node with its functions

### Function-Related Actions
- `_createFunctionId`: Creates a node for the function name
- `_setConstructor`: Marks the function as a constructor
- `_createFunctionSignature`: Creates a signature with return type and parameters 
- `_createConstructorSignature`: Creates a constructor signature with parameters
- `_createFunctionBody`: Creates a function body node containing statements
- `_createFunction`: Creates a FUNCTION node with signature and body
- `_createFunctionDeclaration`: Creates a function declaration for member functions

### Member-Related Actions
- `_setVisibility`: Sets the visibility (public/private) for members
- `_addMember`: Adds a member to a class with the current visibility
- `_createAttribute`: Creates an attribute (class variable)

### Variable-Related Actions
- `_setVariableId`: Sets the variable name
- `_setVariableType`: Sets the variable type
- `_createVariable`: Creates a VARIABLE node
- `_createLocalVariable`: Creates a local variable in a function
- `_setTypeInt`, `_setTypeFloat`, `_setTypeCustom`, `_setTypeVoid`: Set specific types
- `_addArrayDimension`, `_addDynamicArrayDimension`: Process array type information
- `_processArraySize`: Process array size information

### Statement-Related Actions
- `_addBodyStatement`: Adds a statement to a function body
- `_createExpressionStatement`: Creates an expression statement
- `_createIfStatement`: Creates if-statement with condition and branches
- `_createWhileStatement`: Creates while-loop statement
- `_createReadStatement`: Creates read statement
- `_createWriteStatement`: Creates write statement
- `_createReturnStatement`: Creates return statement
- `_createBlock`: Creates a block of statements
- `_createSingleStatement`: Creates a single statement block
- `_addStatement`: Adds a statement to a block

### Expression-Related Actions
- `_pushIdentifier`, `_setSelfIdentifier`: Handles variable references
- `_pushFloatLiteral`, `_pushIntLiteral`: Handles literal values
- `_setRelop`: Sets relational operator for comparison
- `_createRelationalExpr`: Creates a relational expression
- `_pushAddOp`, `_processAddOp`: Handles addition operators
- `_pushMultOp`, `_processMultOp`: Handles multiplication operators
- `_pushSign`, `_applySign`: Handles sign operators
- `_finishExpression`, `_finishArithExpr`, `_finishTerm`, `_finishFactor`: Complete expression building
- `_handleParenExpr`: Processes parenthesized expressions
- `_createNotExpr`: Creates logical NOT expressions

### Array and Function Call Actions
- `_createArrayIndex`: Creates an array index expression
- `_processArrayAccess`: Processes array access
- `_createFunctionCall`: Creates a function call with arguments
- `_addActualParam`: Adds parameter to function call
- `_continueParamList`: Handles multiple parameters
- `_setParamId`, `_setParamType`: Handle formal parameter information
- `_createFormalParam`: Creates a formal parameter node
- `_addFormalParam`: Adds parameter to function signature

### Object-Oriented Actions
- `_processDotAccess`: Handles object member access with dot notation
- `_pushDotIdentifier`: Pushes identifier after dot notation
- `_processFunctionReturn`: Handles method call return values
- `_createAssignment`: Creates assignment operations
- `_setAssignOperator`: Sets specific assignment operator
