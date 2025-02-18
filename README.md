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
├── CMakeLists.txt
├── data
│   ├── lexnegativegrading.alternative1.outlexerrors
│   ├── lexnegativegrading.alternative1.outlextokens
│   ├── lexnegativegrading.alternative2.outlexerrors
│   ├── lexnegativegrading.alternative2.outlextokens
│   ├── lexnegativegrading.outlexerrors
│   ├── lexnegativegrading.outlextokens
│   ├── lexnegativegrading.src
│   ├── lexpositivegrading.outlexerrors
│   ├── lexpositivegrading.outlextokens
│   ├── lexpositivegrading.sample.outlextokens
│   └── lexpositivegrading.src
├── docs
│   └── LexicalAnalyzerSpecifications.pdf
├── README.md
├── src
│   ├── LexDriver.cpp
│   ├── Scanner.cpp
│   └── Scanner.h
└── tests
    ├── CMakeLists.txt
    ├── CMakeLists.txt.in
    ├── data
    │   ├── invalidnums.src
    │   ├── invalidops.src
    │   ├── morecomments.src
    │   └── validcomments.src
    └── TestScanner.cpp

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
