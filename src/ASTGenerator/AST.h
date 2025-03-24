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
    
    // /**
    //  * @brief Creates a new AST node with the specified type and value.
    //  * @param nodeType The type of node to create.
    //  * @param nodeValue The value to store in the node.
    //  * @return Pointer to the newly created ASTNode.
    //  */
    // static ASTNode* createNode(NodeType nodeType, std::string nodeValue);
    
    /**
     * @brief Creates a new AST node with the specified type, value, and line number.
     * @param nodeType The type of node to create.
     * @param nodeValue The value to store in the node.
     * @param line The line number associated with the node.
     * @return Pointer to the newly created ASTNode.
     */
    ASTNode* createNode(NodeType nodeType, std::string nodeValue, int line = 0);
    
    /**
     * @brief Writes the AST structure to a specified file.
     * @param filename The name of the file to write the AST to.
     */
    void writeToFile(std::string filename);
    
    /**
     * @brief Performs a specified action on the AST.
     * @param action String representing the action to be performed.
     * @param value String representing the value to be used in the action.
     * @param line The line number associated with the action.
     */
    void performAction(std::string action, std::string value, int line);
    
    /**
     * @brief Creates a family node structure with a parent and two children.
     * @param op The node type for the parent.
     * @param kid1 First child node.
     * @param kid2 Second child node.
     * @return Pointer to the parent node with the children attached.
     */
    ASTNode* makeFamily(NodeType op, ASTNode* kid1, ASTNode* kid2);
    
    /**
     * @brief Creates a family node structure with a parent and multiple children (variadic).
     * @param op The node type for the parent.
     * @param kid1 First child node.
     * @param kid2 Second child node.
     * @param rest Remaining child nodes.
     * @return Pointer to the parent node with the children attached.
     */
    template<typename... Args>
    ASTNode* makeFamily(NodeType op, ASTNode* kid1, ASTNode* kid2, Args... rest);
    
    /**
     * @brief Creates a family node structure with a parent and a single child.
     * @param op The node type for the parent.
     * @param kid Single child node.
     * @return Pointer to the parent node with the child attached.
     */
    ASTNode* makeFamily(NodeType op, ASTNode* kid);

    /**
     * @brief Gets the root node of the AST.
     * @return Pointer to the root node of the AST.
     */
    ASTNode* getRoot();
};

#endif // AST_H