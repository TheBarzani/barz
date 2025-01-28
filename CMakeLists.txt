cmake_minimum_required(VERSION 3.12)
project(Barz LANGUAGES CXX)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Define executable
add_executable(lexdriver
    src/Scanner.cpp    # Scanner implementation
    src/LexDriver.cpp          # Driver code
)

# Include directories (headers in include/Scanner)
target_include_directories(lexdriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add the tests directory
add_subdirectory(tests)

