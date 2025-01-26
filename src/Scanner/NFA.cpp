#include "Scanner/NFA.h"
#include <memory>

// Single-character NFA (e.g., 'a')
NFA::NFA(char c) {
    start = std::make_shared<NFAState>();
    end = std::make_shared<NFAState>();
    start->transitions.emplace_back(c, end);
    end->isFinal = true; // Mark end state as final
}

// Concatenation: A followed by B
NFA NFA::concat(NFA& a, NFA& b) {
    a.end->transitions.emplace_back('\0', b.start);
    a.end->isFinal = false; // A's end is no longer final
    NFA result;
    result.start = a.start;
    result.end = b.end; // B's end remains final
    return result;
}

// Union: A or B
NFA NFA::union_(NFA& a, NFA& b) {
    auto newStart = std::make_shared<NFAState>();
    auto newEnd = std::make_shared<NFAState>();
    newEnd->isFinal = true; // Union's end is final

    newStart->transitions.emplace_back('\0', a.start);
    newStart->transitions.emplace_back('\0', b.start);

    a.end->transitions.emplace_back('\0', newEnd);
    b.end->transitions.emplace_back('\0', newEnd);
    a.end->isFinal = b.end->isFinal = false;

    NFA result;
    result.start = newStart;
    result.end = newEnd;
    return result;
}

// Kleene Star: A*
NFA NFA::kleeneStar(NFA& a) {
    auto newStart = std::make_shared<NFAState>();
    auto newEnd = std::make_shared<NFAState>();
    newEnd->isFinal = true; // Kleene star's end is final

    newStart->transitions.emplace_back('\0', a.start);
    newStart->transitions.emplace_back('\0', newEnd);

    a.end->transitions.emplace_back('\0', a.start);
    a.end->transitions.emplace_back('\0', newEnd);
    a.end->isFinal = false;

    NFA result;
    result.start = newStart;
    result.end = newEnd;
    return result;
}