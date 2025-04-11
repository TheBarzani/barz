#include "ASTNode.h"
#include "Semantics/Visitor.h"
#include <iostream>

int ASTNode::nodeCount = 0;

ASTNode::ASTNode()
    : leftMostChild(nullptr),
      leftMostSibling(nullptr), 
      rightSibling(nullptr),
      parent(nullptr),
      nodeType(NodeType::EMPTY),
      nodeValue(""),
      nodeNumber(nodeCount)
{
    nodeCount++;
}

ASTNode::ASTNode(NodeType nodeType, std::string nodeValue)
    : leftMostChild(nullptr),
      leftMostSibling(nullptr), 
      rightSibling(nullptr),
      parent(nullptr),
      nodeType(nodeType),
      nodeValue(nodeValue),
      nodeNumber(nodeCount)   
{
    nodeCount++;
}

ASTNode::~ASTNode()
{
}

ASTNode* ASTNode::getLeftMostChild()
{
    return leftMostChild;
}

void ASTNode::setLeftMostChild(ASTNode* leftMost)
{
    this->leftMostChild = leftMost;
}

ASTNode* ASTNode::getLeftMostSibling()
{
    return leftMostSibling == nullptr ? this : leftMostSibling;
}

void ASTNode::setLeftMostSibling(ASTNode* leftMost)
{
    this->leftMostSibling = leftMost;
}

ASTNode* ASTNode::getRightSibling()
{
    return rightSibling;
}

void ASTNode::setRightSibling(ASTNode* rightSibling)
{
    this->rightSibling = rightSibling;
}

ASTNode* ASTNode::getParent()
{
    return parent;
}

void ASTNode::setParent(ASTNode* parent)
{
    this->parent = parent;
}
NodeType ASTNode::getNodeEnum()
{
    return nodeType;
}

std::string ASTNode::getNodeType()
{
    switch (nodeType)
    {
        case NodeType::EMPTY:               return "EMPTY";
        case NodeType::PROGRAM:             return "PROGRAM";
        case NodeType::CLASS_LIST:          return "CLASS_LIST";
        case NodeType::FUNCTION_LIST:       return "FUNCTION_LIST";
        case NodeType::IMPLEMENTATION_LIST: return "IMPLEMENTATION_LIST";
        case NodeType::CLASS:               return "CLASS";
        case NodeType::FUNCTION:            return "FUNCTION";
        case NodeType::IMPLEMENTATION:      return "IMPLEMENTATION";
        case NodeType::CLASS_ID:            return "CLASS_ID";
        case NodeType::INHERITANCE_LIST:    return "INHERITANCE_LIST";
        case NodeType::INHERITANCE_ID:      return "INHERITANCE_ID";
        case NodeType::VISIBILITY:          return "VISIBILITY";
        case NodeType::MEMBER_LIST:         return "MEMBER_LIST";
        case NodeType::MEMBER:              return "MEMBER";
        case NodeType::VARIABLE:            return "VARIABLE";
        case NodeType::FUNCTION_ID:         return "FUNCTION_ID";
        case NodeType::FUNCTION_SIGNATURE:  return "FUNCTION_SIGNATURE";
        case NodeType::FUNCTION_BODY:       return "FUNCTION_BODY";
        case NodeType::CONSTRUCTOR_SIGNATURE: return "CONSTRUCTOR_SIGNATURE";
        case NodeType::LOCAL_VARIABLE:      return "LOCAL_VARIABLE";
        case NodeType::BLOCK:               return "BLOCK";
        case NodeType::IF_STATEMENT:        return "IF_STATEMENT";
        case NodeType::WHILE_STATEMENT:     return "WHILE_STATEMENT";
        case NodeType::RELATIONAL_EXPR:     return "RELATIONAL_EXPR";
        case NodeType::ASSIGNMENT:          return "ASSIGNMENT";
        case NodeType::FUNCTION_DECLARATION: return "FUNCTION_DECLARATION";
        case NodeType::ATTRIBUTE:           return "ATTRIBUTE";
        case NodeType::SINGLE_STATEMENT:    return "SINGLE_STATEMENT";
        case NodeType::EXPRESSION_STATEMENT: return "EXPRESSION_STATEMENT";
        case NodeType::READ_STATEMENT:      return "READ_STATEMENT";
        case NodeType::WRITE_STATEMENT:     return "WRITE_STATEMENT";
        case NodeType::RETURN_STATEMENT:    return "RETURN_STATEMENT";
        case NodeType::ASSIGN_OP:           return "ASSIGN_OP";
        case NodeType::REL_OP:              return "REL_OP";
        case NodeType::ADD_OP:              return "ADD_OP";
        case NodeType::MULT_OP:             return "MULT_OP";
        case NodeType::IDENTIFIER:          return "IDENTIFIER";
        case NodeType::SELF_IDENTIFIER:     return "SELF_IDENTIFIER";
        case NodeType::TYPE:                return "TYPE";
        case NodeType::ARRAY_DIMENSION:     return "ARRAY_DIMENSION";
        case NodeType::PARAM:               return "PARAMATER";
        case NodeType::FUNCTION_CALL:       return "FUNCTION_CALL";
        case NodeType::ARRAY_ACCESS:        return "ARRAY_ACCESS";
        case NodeType::DOT_IDENTIFIER:      return "DOT_IDENTIFIER";
        case NodeType::FACTOR:              return "FACTOR";
        case NodeType::TERM:                return "TERM";
        case NodeType::ARITH_EXPR:          return "ARITH_EXPR";
        case NodeType::EXPR:                return "EXPR";
        case NodeType::IMPLEMENTATION_ID:   return "IMPLEMENTATION_ID";
        case NodeType::VARIABLE_ID:         return "VARIABLE_ID";
        case NodeType::PARAM_LIST:          return "PARAM_LIST";
        case NodeType::PARAM_ID:            return "PARAM_ID";
        case NodeType::FLOAT:               return "FLOAT";
        case NodeType::INT:                 return "INTEGER";
        case NodeType::STATEMENTS_LIST:     return "STATEMENTS_LIST";
        case NodeType::IMPLEMENTATION_FUNCTION_LIST: return "IMPLEMENTATION_FUNCTION_LIST";
        case NodeType::CONDITION:           return "CONDITION";
        case NodeType::ARRAY_TYPE:          return "ARRAY_TYPE";
        case NodeType::DOT_ACCESS:          return "DOT_ACCESS";
        case NodeType::INDEX_LIST:          return "INDEX_LIST";
        case NodeType::DIM_LIST:            return "DIM_LIST";
        default:
            // TODO: throw an exception when this happens
            return "UNKNOWN";
    }
}

void ASTNode::setNodeType(NodeType nodeType)
{
    this->nodeType = nodeType;
}

std::string ASTNode::getNodeValue()
{
    return nodeValue;
}

void ASTNode::setNodeValue(std::string nodeValue)
{
    this->nodeValue = nodeValue;
}

void ASTNode::adoptChildren(ASTNode* child)
{
    if (child == nullptr) {
        return;
    }

    if (this->leftMostChild != nullptr) {
        this->leftMostChild->makeSiblings(child);
    } else {
        // Set leftmost child
        ASTNode* ysibs = child->getLeftMostSibling();
        this->leftMostChild = ysibs;
        ysibs->setParent(this);
        
        // Set parent for the right siblings
        while (ysibs != nullptr) {
            ysibs->parent = this;
            ysibs = ysibs->rightSibling;
        }
    }
}

ASTNode* ASTNode::makeSiblings(ASTNode* sibling)
{
    if (sibling == nullptr) {
        return this;
    }

    // Find the rightmost node in this list
    ASTNode* xsibs = this;
    while (xsibs->rightSibling != nullptr) {
        xsibs = xsibs->rightSibling;
    }

    // Join the lists
    ASTNode* ysibs = sibling->getLeftMostSibling();
    xsibs->rightSibling = ysibs;

    // Set pointers for the new siblings
    ASTNode* leftmost = xsibs->leftMostSibling == nullptr ? xsibs : xsibs->leftMostSibling;
    ysibs->leftMostSibling = leftmost;
    ysibs->parent = xsibs->parent;
    
    while (ysibs->rightSibling != nullptr) {
        ysibs = ysibs->rightSibling;
        ysibs->leftMostSibling = leftmost;
        ysibs->parent = xsibs->parent;
    }
    
    return ysibs;
}

void ASTNode::accept(Visitor* visitor) {
    visitor->visit(this);
}

// Implement the new methods

int ASTNode::getLineNumber() const {
    return lineNumber;
}

void ASTNode::setLineNumber(int line) {
    lineNumber = line;
}

void ASTNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string ASTNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return "";
}

bool ASTNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}