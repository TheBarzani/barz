#pragma once
#include "Scanner/NFA.h"
#include <set>
#include <map>

using StateSet = std::set<std::shared_ptr<NFAState>>;

struct DFAState {
    StateSet nfaStates;
    std::map<char, int> transitions;
    bool isFinal = false;
};

class DFA {
public:
    std::vector<DFAState> states;
    static DFA fromNFA(const NFA& nfa);
};