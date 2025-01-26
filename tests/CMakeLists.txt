cmake_minimum_required(VERSION 3.10)
project(CompilerTests)

# Set C++ standard and enable testing
set(CMAKE_CXX_STANDARD 17)
enable_testing()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../include
)

# Source files from the main project (adjust paths as needed)
set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/../src/Scanner/NFA.cpp
    ${CMAKE_SOURCE_DIR}/../src/Scanner/RegexParser.cpp
)

# Define test executables
# ------------------------------------------------

# Test 1: NFA construction and simulation
add_executable(TestNFA
    TestNFA.cpp
    ${SRC_FILES}
)
target_include_directories(TestNFA PRIVATE ${CMAKE_SOURCE_DIR}/../include/Scanner)
add_test(NAME test_nfa COMMAND TestNFA)

# Test 2: RegexParser and NFA integration
add_executable(TestRegexParser
    TestRegexParser.cpp
    ${SRC_FILES}
)
target_include_directories(TestRegexParser PRIVATE ${CMAKE_SOURCE_DIR}/../include/Scanner)
add_test(NAME test_regexparser COMMAND TestRegexParser)

# Optional: Add other tests here
# ------------------------------------------------

# Run tests with `ctest` after building