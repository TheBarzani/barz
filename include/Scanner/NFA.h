#ifndef NFA_H
#define NFA_H

#include <memory>
#include <vector>
#include <utility>

struct NFAState {
    bool isFinal = false;
    std::vector<std::pair<char, std::shared_ptr<NFAState>>> transitions;
};

class NFA {
public:
    std::shared_ptr<NFAState> start;
    std::shared_ptr<NFAState> end;

    // Default constructor (required for static methods)
    NFA() = default;

    // Constructor for single-character NFA
    explicit NFA(char c);

    // Thompson's construction operations
    static NFA concat(NFA& a, NFA& b);
    static NFA union_(NFA& a, NFA& b);
    static NFA kleeneStar(NFA& a);
};

#endif // NFA_H