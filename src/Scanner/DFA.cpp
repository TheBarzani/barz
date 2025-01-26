#include "DFA.h"
#include <algorithm>

static StateSet epsilonClosure(const StateSet& states) {
    StateSet closure = states;
    // ... (epsilon closure implementation)
    return closure;
}

DFA DFA::fromNFA(const NFA& nfa) {
    DFA dfa;
    // ... (subset construction implementation)
    return dfa;
}