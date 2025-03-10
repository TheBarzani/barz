#ifndef AST_H
#define AST_H

#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <ASTGenerator/ASTNode.h>

/**
 * @class AST
 * @brief Represents an Abstract Syntax Tree for parsing and interpreting code.
 * 
 * The AST class (AST Factory) provides functionality to build, navigate, and manipulate an Abstract Syntax Tree
 * structure. It maintains the root node, tracks the current position in the tree during construction,
 * and offers methods to create nodes, perform actions, and output the tree to a file.
 */
class AST {
private:
    /**
     * @var root
     * @brief The root node of the Abstract Syntax Tree.
     */
    ASTNode *root;
    
    /**
     * @var current
     * @brief Pointer to the current node being processed or manipulated.
     */
    ASTNode *current;
    
    /**
     * @var ASTStack
     * @brief Stack used to track nodes during tree traversal and construction.
     */
    std::vector<ASTNode*> ASTStack;
    
public:
    /**
     * @brief Constructs a new empty AST object.
     */
    AST();
    
    /**
     * @brief Destroys the AST object and frees all allocated memory.
     */
    ~AST();
    
    /**
     * @brief Creates a new AST node with the specified type and value.
     * @param nodeType The type of node to create.
     * @param nodeValue The value to store in the node.
     * @return Pointer to the newly created ASTNode.
     */
    static ASTNode* createNode(NodeType nodeType, std::string nodeValue);
    
    /**
     * @brief Writes the AST structure to a specified file.
     * @param filename The name of the file to write the AST to.
     */
    void writeToFile(std::string filename);
    
    /**
     * @brief Performs a specified action on the AST.
     * @param action String representing the action to be performed.
     */
    void performAction(std::string action);
};

#endif // AST_H