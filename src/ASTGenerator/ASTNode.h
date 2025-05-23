/**
 * @file ASTNode.h
 * @brief Defines the ASTNode class and the NodeType enumeration.
 */

#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <unordered_map>

/**
 * @enum NodeType
 * @brief Identifies the type of a node in the abstract syntax tree.
 */
enum class NodeType
{
    EMPTY,
    PROGRAM,
    CLASS_LIST,
    FUNCTION_LIST,
    IMPLEMENTATION_LIST,
    CLASS,
    FUNCTION,
    IMPLEMENTATION,
    CLASS_ID,
    INHERITANCE_LIST,
    INHERITANCE_ID,
    VISIBILITY,
    MEMBER_LIST,
    MEMBER,
    VARIABLE,
    FUNCTION_ID,
    FUNCTION_SIGNATURE,
    FUNCTION_BODY,
    CONSTRUCTOR_SIGNATURE,
    LOCAL_VARIABLE,
    BLOCK,
    IF_STATEMENT,
    WHILE_STATEMENT,
    RELATIONAL_EXPR,
    ASSIGNMENT,
    FUNCTION_DECLARATION,
    ATTRIBUTE,
    SINGLE_STATEMENT,
    EXPRESSION_STATEMENT,
    READ_STATEMENT,
    WRITE_STATEMENT,
    RETURN_STATEMENT,
    ASSIGN_OP,
    REL_OP,
    ADD_OP,
    MULT_OP,
    IDENTIFIER,
    SELF_IDENTIFIER,
    TYPE,
    ARRAY_DIMENSION,
    PARAM,
    FUNCTION_CALL,
    ARRAY_ACCESS,
    DOT_IDENTIFIER,
    FACTOR,
    TERM,
    ARITH_EXPR,
    EXPR,
    IMPLEMENTATION_ID,
    VARIABLE_ID,
    PARAM_LIST,
    PARAM_ID,
    FLOAT,
    INT,
    STATEMENTS_LIST,
    IMPLEMENTATION_FUNCTION_LIST,
    CONDITION,
    ARRAY_TYPE,
    DOT_ACCESS,
    INDEX_LIST,
    DIM_LIST,
};

/**
 * @class Visitor
 * @brief Represents a visitor that traverses an abstract syntax tree (AST).
 * @note This class is defined in the Visitor.h file. This is just a forward declaration.
 */
class Visitor;

/**
 * @class ASTNode
 * @brief Represents a node in an abstract syntax tree (AST).
 */
class ASTNode
{
private:
    ASTNode *leftMostChild;   ///< Pointer to the leftmost child node.                
    ASTNode *leftMostSibling; ///< Pointer to the leftmost sibling node.
                              ///< leftMostSibling points to itself if the node is the left most in an AST.
    ASTNode *rightSibling;    ///< Pointer to the right sibling node.
    ASTNode *parent;          ///< Pointer to the parent node.
    NodeType nodeType;        ///< Type of this AST node.
    std::string nodeValue;    ///< Value or content of this AST node.
    int nodeNumber;           ///< Unique number assigned to this node.
    static int nodeCount;     ///< Counter for generating unique node numbers.
    int lineNumber = 0;       ///< Line number in the source code
    std::unordered_map<std::string, std::string> metadata;

public:
    /**
     * @brief Default constructor for ASTNode.
     */
    ASTNode();

    /**
     * @brief Constructor for ASTNode.
     * @param nodeType The type of the node.
     * @param nodeValue The value of the node.
     */
    ASTNode(NodeType nodeType, std::string nodeValue);

    /**
     * @brief Destructor for ASTNode.
     */
    ~ASTNode();

    /**
     * @brief Gets the leftmost child node.
     * @return Pointer to the leftmost child node.
     */
    ASTNode *getLeftMostChild();

    /**
     * @brief Sets the leftmost child node.
     * @param leftMost Pointer to the node to be set as leftmost child.
     */
    void setLeftMostChild(ASTNode *leftMost);

        /**
     * @brief Gets the leftmost child node.
     * @return Pointer to the leftmost child node.
     */
    ASTNode *getLeftMostSibling();

    /**
     * @brief Sets the leftmost child node.
     * @param leftMost Pointer to the node to be set as leftmost child.
     */
    void setLeftMostSibling(ASTNode *leftMost);

    /**
     * @brief Gets the right sibling node.
     * @return Pointer to the right sibling node.
     */
    ASTNode *getRightSibling();

    /**
     * @brief Sets the right sibling node.
     * @param rightSibling Pointer to the node to be set as right sibling.
     */
    void setRightSibling(ASTNode *rightSibling);

    /**
     * @brief Gets the parent node.
     * @return Pointer to the parent node.
     */
    ASTNode *getParent();

    /**
     * @brief Sets the parent node.
     * @param parent Pointer to the node to be set as parent.
     */
    void setParent(ASTNode *parent);

    /**
     * @brief Gets the node's enum value
     * @return The node's enum value
     */
    NodeType getNodeEnum();
    /**
     * @brief Gets the node type as string.
     * @return String representation of the node type.
     */
    std::string getNodeType();

    /**
     * @brief Sets the node type.
     * @param nodeType The type to be assigned to this node.
     */
    void setNodeType(NodeType nodeType);

    /**
     * @brief Gets the node value.
     * @return String value or content of this node.
     */
    std::string getNodeValue();

    /**
     * @brief Sets the node value.
     * @param nodeValue String value to be assigned to this node.
     */
    void setNodeValue(std::string nodeValue);

    /**
     * @brief Gets the unique node number.
     * @return Unique number assigned to this node.
     */
    int getNodeNumber() const { return nodeNumber; }

    /**
     * @brief Adds a child node to this node.
     * @param child Pointer to the child node to be added.
     */
    void adoptChildren(ASTNode* child);

    /**
     * @brief Adds a sibling node to this node.
     * @param sibling Pointer to the sibling node to be added.
     * @return Pointer to the rightmost sibling node.
     */
    ASTNode* makeSiblings(ASTNode* sibling);

    /**
     * @brief Accepts a visitor.
     * @param visitor Pointer to the visitor.
     */
    void accept(Visitor* visitor);

    /**
     * @brief Gets the line number in the source code.
     * @return Line number where this node was defined.
     */
    int getLineNumber() const;

    /**
     * @brief Sets the line number for this node.
     * @param line Line number in source code.
     */
    void setLineNumber(int line);

    /**
     * @brief Sets metadata for the node.
     * @param key The metadata key.
     * @param value The metadata value.
     */
    void setMetadata(const std::string& key, const std::string& value);

    /**
     * @brief Gets metadata from the node.
     * @param key The metadata key.
     * @return The metadata value or empty string if not found.
     */
    std::string getMetadata(const std::string& key) const;
    
    /**
     * @brief Checks if metadata exists.
     * @param key The metadata key.
     * @return True if the metadata exists.
     */
    bool hasMetadata(const std::string& key) const;
};

#endif // ASTNODE_H