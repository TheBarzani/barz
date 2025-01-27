cmake_minimum_required(VERSION 3.10)
project(ScannerTests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Add the source directory
include_directories(${CMAKE_SOURCE_DIR}/../src)

# Add the test executable and include the Scanner source files
add_executable(TestScanner
    TestScanner.cpp
    ../src/Scanner.cpp  # Add the Scanner implementation file(s)
)

# Link the test executable against gtest_main
target_link_libraries(TestScanner gtest_main)

# Add a test to CTest
add_test(NAME TestScanner COMMAND TestScanner)

# Disable maybe-uninitialized warning for Google Test
set_target_properties(gtest PROPERTIES COMPILE_FLAGS "-Wno-maybe-uninitialized")
set_target_properties(gtest_main PROPERTIES COMPILE_FLAGS "-Wno-maybe-uninitialized")