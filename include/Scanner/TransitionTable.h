#pragma once
#include "Scanner/DFA.h"
#include "Scanner/Scanner.h"
#include <vector>
#include <map>

class TransitionTable {
public:
    std::vector<std::map<char, int>> table;
    std::map<int, TokenType> acceptStates;
    
    void buildFromDFA(const DFA& dfa, TokenType tokenType);
    Token process(std::istream& input, int& line, int& column);
};