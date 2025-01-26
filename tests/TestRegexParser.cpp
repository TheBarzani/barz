#include "Scanner/RegexParser.h"
#include "Scanner/NFA.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

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

// Helper function to test a regex against multiple inputs
void testRegex(const string& regex, const vector<pair<string, bool>>& testCases) {
    try {
        NFA nfa = RegexParser::parse(regex);
        cout << "Testing regex: " << regex << endl;
        for (const auto& [input, expected] : testCases) {
            bool result = simulateNFA(nfa, input);
            cout << "  Input: '" << input << "'\tExpected: " 
                 << (expected ? "Accept" : "Reject") 
                 << "\tActual: " << (result ? "Accept" : "Reject")
                 << (result == expected ? " [PASS]" : " [FAIL]") << endl;
        }
    } catch (const exception& e) {
        cerr << "Error parsing regex '" << regex << "': " << e.what() << endl;
    }
    cout << endl;
}

// Test invalid regexes
void testInvalidRegex(const string& regex) {
    try {
        NFA nfa = RegexParser::parse(regex);
        cerr << "Regex '" << regex << "' parsed unexpectedly [FAIL]" << endl;
    } catch (const exception& e) {
        cout << "Invalid regex '" << regex << "' correctly threw error: " 
             << e.what() << " [PASS]" << endl;
    }
}

int main() {
    // Test 1: Single character
    testRegex("a", {
        {"a", true},
        {"b", false},
        {"", false},
        {"aa", false}
    });

    // Test 2: Concatenation
    testRegex("ab", {
        {"ab", true},
        {"a", false},
        {"b", false},
        {"aba", false}
    });

    // Test 3: Union
    testRegex("a|b", {
        {"a", true},
        {"b", true},
        {"c", false},
        {"ab", false}
    });

    // Test 4: Kleene Star
    testRegex("a*", {
        {"", true},
        {"a", true},
        {"aa", true},
        {"aaa", true},
        {"b", false}
    });

    // Test 5: Parentheses and grouping
    testRegex("(a|b)c", {
        {"ac", true},
        {"bc", true},
        {"a", false},
        {"c", false},
        {"ab", false}
    });

    // Test 6: Complex combination
    testRegex("a(b|c)*", {
        {"a", true},
        {"ab", true},
        {"ac", true},
        {"abbc", true},
        {"b", false},
        {"abcx", false}
    });

    // Test 7: Invalid regexes
    testInvalidRegex("a|");
    testInvalidRegex("(a|b");
    testInvalidRegex("*a");

    return 0;
}