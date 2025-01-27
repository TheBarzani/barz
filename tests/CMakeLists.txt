cmake_minimum_required(VERSION 3.10)
project(CompilerTests)

# Set C++ standard and enable testing
set(CMAKE_CXX_STANDARD 17)
enable_testing()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../src
)

# Source files from the main project (adjust paths as needed)
set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/../src/Scanner.cpp
)

# Define test executables
# ------------------------------------------------

# Scanner tests
add_executable(TestScanner
    TestScanner.cpp
    ${SRC_FILES}
)
target_include_directories(TestScanner PRIVATE ${CMAKE_SOURCE_DIR}/../src)
add_test(NAME test_scanner COMMAND TestScanner)

# Optional: Add other tests here
# ------------------------------------------------

# Run tests with `ctest` after building