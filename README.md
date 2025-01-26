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
├── CMakeLists.txt <-- configures dependencies, platform specifics and output paths
├── README.md
├── .github
├── build/ <-- Build files to go here (source-code excluded)
│   ├── Executable-OutputA
│   └── Library-OutputA
├── cmake/ <--  To abstract away all my CMake helper scripts
│   └── *.cmake
├── data/ <-- If I use data, it goes here
├── docs/ <-- I document my stuff here
│   └── Doxyfile
├── examples/ <-- example code goes here
├── external/ <-- Third-party projects and libraries go here
│   ├── External-ProjectA
│   └── External-ProjectB
├── include/ <-- Public headers go here
│   ├── *.h
│   ├── ExecutableA
│   │   └── *.h
│   └── ExecutableB
│       └── *.h
├── lib/ <-- Libraries that I could potentially create
│   ├── LibraryA
│   │   └── *source-code*
│   └── LibraryB
│       └── *source-code*
├── src/ <-- source-code goes here 
│   ├── CMakeLists.txt <-- configures the library you are going to build
│   ├── *.cpp
│   ├── ExecutableA
│   │   ├── CMakeLists.txt
│   │   ├── *.h
│   │   └── *.cpp
│   └── ExecutableB
│       ├── CMakeLists.txt
│       ├── *.h
│       └── *.cpp
└── tests/ <-- tests are performed here
    ├── CMakeLists.txt <-- configures the Test executables and test-cases
    ├── data
    │   └── testdata
    └── testcase.cpp

```

## Building and Running the Project with CMake

Follow these steps to build and run the `Driver` executable using CMake:

1. **Create a build directory**:
   Open a terminal and navigate to your project directory. Then create a build directory.

   ```sh
   mkdir build
   cd build



## Building and Running the Project with CMake

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
   ./Driver
   ```

