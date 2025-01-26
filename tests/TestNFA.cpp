#include "Scanner/NFA.h"
#include <iostream>
#include <algorithm>

// Compute epsilon closure of states
std::vector<std::shared_ptr<NFAState>> epsilonClosure(const std::vector<std::shared_ptr<NFAState>>& states) {
    std::vector<std::shared_ptr<NFAState>> closure = states;
    size_t index = 0;
    while (index < closure.size()) {
        auto state = closure[index];
        for (auto& transition : state->transitions) {
            if (transition.first == '\0') { // Epsilon transition
                auto nextState = transition.second;
                if (std::find(closure.begin(), closure.end(), nextState) == closure.end()) {
                    closure.push_back(nextState);
                }
            }
        }
        index++;
    }
    return closure;
}

// Simulate NFA execution with epsilon closure
bool simulateNFA(const NFA& nfa, const std::string& input) {
    std::vector<std::shared_ptr<NFAState>> currentStates = epsilonClosure({nfa.start});

    for (char c : input) {
        std::vector<std::shared_ptr<NFAState>> nextStates;
        for (auto& state : currentStates) {
            for (auto& transition : state->transitions) {
                if (transition.first == c) {
                    nextStates.push_back(transition.second);
                }
            }
        }
        currentStates = epsilonClosure(nextStates);
    }

    for (auto& state : currentStates) {
        if (state->isFinal) return true;
    }
    return false;
}


int main() {
    // Test single character
    NFA a('a');
    std::cout << "Test 'a' matches 'a': " 
              << (simulateNFA(a, "a") ? "Pass" : "Fail") << std::endl;

    // Test concatenation
    NFA b('b');
    NFA ab = NFA::concat(a, b);
    std::cout << "Test 'ab' matches 'ab': " 
              << (simulateNFA(ab, "ab") ? "Pass" : "Fail") << std::endl;

    // Test union
    NFA a_or_b = NFA::union_(a, b);
    std::cout << "Test 'a' matches union: " 
              << (simulateNFA(a_or_b, "a") ? "Pass" : "Fail") << std::endl;
    std::cout << "Test 'b' matches union: " 
              << (simulateNFA(a_or_b, "b") ? "Pass" : "Fail") << std::endl;

    // Test Kleene star
    NFA a_star = NFA::kleeneStar(a);
    std::cout << "Test '' matches a*: " 
              << (simulateNFA(a_star, "") ? "Pass" : "Fail") << std::endl;
    std::cout << "Test 'aaa' matches a*: " 
              << (simulateNFA(a_star, "aaa") ? "Pass" : "Fail") << std::endl;

    return 0;
}