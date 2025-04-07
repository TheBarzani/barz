cmake_minimum_required(VERSION 3.12)
project(Barz LANGUAGES CXX)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug build settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Define executable
add_executable(lexdriver
    src/Scanner/Scanner.cpp    # Scanner implementation
    src/LexDriver.cpp          # Driver code
)
add_executable(parsedriver
    src/Scanner/Scanner.cpp         # Scanner implementation
    src/Parser/Parser.cpp           # Parser implementation
    src/ASTGenerator/AST.cpp        # AST generation code
    src/ASTGenerator/ASTNode.cpp    # AST node code
    src/ASTDriver.cpp               # Driver code
)
add_executable(astdriver
    src/Scanner/Scanner.cpp         # Scanner implementation
    src/Parser/Parser.cpp           # Parser implementation
    src/ASTGenerator/AST.cpp        # AST generation code
    src/ASTGenerator/ASTNode.cpp    # AST node code
    src/ASTDriver.cpp               # Driver code
)
add_executable(semanticanalyzerdriver
    src/Scanner/Scanner.cpp         # Scanner implementation
    src/Parser/Parser.cpp           # Parser implementation
    src/ASTGenerator/AST.cpp        # AST generation code
    src/ASTGenerator/ASTNode.cpp    # AST node code
    src/Semantics/SymbolTableVisitor.cpp # Symbol Table Generation code
    src/Semantics/SemanticCheckingVisitor.cpp # Semantics Checking code
    src/SemanticsDriver.cpp         # Driver code
)

add_executable(codegendriver
    src/Scanner/Scanner.cpp         # Scanner implementation
    src/Parser/Parser.cpp           # Parser implementation
    src/ASTGenerator/AST.cpp        # AST generation code
    src/ASTGenerator/ASTNode.cpp    # AST node code
    src/Semantics/SymbolTableVisitor.cpp # Symbol Table Generation code
    src/Semantics/SemanticCheckingVisitor.cpp # Semantics Checking code
    src/CodeGenerator/MemSizeVisitor.cpp # Memory size calculation code
    src/CodeGenDriver.cpp         # Driver code
)


# Include source directories 
target_include_directories(lexdriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(parsedriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(astdriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(semanticanalyzerdriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(codegendriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
# Include CSV file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/syntactical_analysis/LL1_parsing_table.csv ${CMAKE_BINARY_DIR}/LL1_parsing_table.csv COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/ast_generation/attribute_grammar_parsing_table.csv ${CMAKE_BINARY_DIR}/attribute_grammar_parsing_table.csv COPYONLY)

# Add the tests directory
add_subdirectory(tests)

