cmake_minimum_required(VERSION 3.10)
project(barz)

set(CMAKE_CXX_STANDARD 17)

add_executable(lexdriver
    Scanner/Sanner.cpp
    Scanner/RegexParser.cpp
    Scanner/NFAToDFAConverter.cpp
    Scanner/TransitionTable.cpp
    LexDriver.cpp
)

target_include_directories(lexdriver PRIVATE include)