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
