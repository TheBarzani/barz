#include "RegexParser.h"
#include <stdexcept>

using namespace std;

// Tokenize the regex string into recognized operators/literals
vector<RegexParser::Token> RegexParser::tokenize(const string& regex) {
    vector<Token> tokens;
    bool escape = false;

    for (size_t i = 0; i < regex.size(); i++) {
        char c = regex[i];

        if (escape) { // Handle escaped characters (not used in this assignment)
            tokens.push_back({c, LITERAL});
            escape = false;
        } else {
            switch (c) {
                case '|': tokens.push_back({c, UNION}); break;
                case '*': tokens.push_back({c, STAR}); break;
                case '(': tokens.push_back({c, LPAREN}); break;
                case ')': tokens.push_back({c, RPAREN}); break;
                case '\\': escape = true; break;
                default: tokens.push_back({c, LITERAL}); break;
            }
        }
    }
    return tokens;
}

// Get operator precedence for Shunting Yard
int RegexParser::precedence(TokenType op) {
    static unordered_map<TokenType, int> prec = {
        {UNION, 1}, {CONCAT, 2}, {STAR, 3}
    };
    return prec[op];
}

// Convert infix regex to postfix notation with implicit concatenation
vector<RegexParser::Token> RegexParser::infixToPostfix(const vector<Token>& tokens) {
    vector<Token> output;
    stack<Token> opStack;
    TokenType prevType = LPAREN; // Track previous token for implicit concat

    for (const Token& t : tokens) {
        // Insert CONCAT operator where needed
        if ((prevType == LITERAL || prevType == RPAREN || prevType == STAR) && 
            (t.type == LITERAL || t.type == LPAREN)) {
            opStack.push({'.', CONCAT});
            prevType = CONCAT;
        }

        switch (t.type) {
            case LITERAL:
            case LPAREN:
                if (t.type == LPAREN) opStack.push(t);
                else output.push_back(t);
                break;

            case RPAREN:
                while (!opStack.empty() && opStack.top().type != LPAREN) {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                if (opStack.empty()) throw runtime_error("Mismatched parentheses");
                opStack.pop(); // Remove LPAREN
                break;

            case UNION:
            case STAR:
            case CONCAT:
                while (!opStack.empty() && 
                       precedence(opStack.top().type) >= precedence(t.type)) {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                opStack.push(t);
                break;
        }

        prevType = t.type;
    }

    // Add remaining operators
    while (!opStack.empty()) {
        output.push_back(opStack.top());
        opStack.pop();
    }

    return output;
}

// Build NFA from postfix expression using Thompson's algorithm
NFA RegexParser::parse(const string& regex) {
    vector<Token> tokens = tokenize(regex);
    vector<Token> postfix = infixToPostfix(tokens);
    stack<NFA> nfaStack;

    for (const Token& t : postfix) {
        switch (t.type) {
            case LITERAL: {
                NFA nfa(t.symbol); // Basic single-character NFA
                nfaStack.push(nfa);
                break;
            }

            case CONCAT: {
                if (nfaStack.size() < 2) throw runtime_error("Invalid regex");
                NFA b = nfaStack.top(); nfaStack.pop();
                NFA a = nfaStack.top(); nfaStack.pop();
                nfaStack.push(NFA::concat(a, b));
                break;
            }

            case UNION: {
                if (nfaStack.size() < 2) throw runtime_error("Invalid regex");
                NFA b = nfaStack.top(); nfaStack.pop();
                NFA a = nfaStack.top(); nfaStack.pop();
                nfaStack.push(NFA::union_(a, b));
                break;
            }

            case STAR: {
                if (nfaStack.empty()) throw runtime_error("Invalid regex");
                NFA a = nfaStack.top(); nfaStack.pop();
                nfaStack.push(NFA::kleeneStar(a));
                break;
            }

            default:
                throw runtime_error("Unexpected token in postfix");
        }
    }

    if (nfaStack.size() != 1) throw runtime_error("Invalid regex");
    return nfaStack.top();
}