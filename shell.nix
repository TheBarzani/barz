{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # Compilers and build tools
    gcc
    ninja
    cmake
    
    # Development tools
    clang-tools
    codespell
    conan
    cppcheck
    doxygen
    gtest
    lcov
    vcpkg
    vcpkg-tool
  ];
  
  shellHook = ''
    export SHELL=$(which zsh)
    export GDB_PATH=$(which gdb)
    echo "GDB path: $GDB_PATH"  # This will show the path when entering the shell
  '';
}