#include "ASTNode.h"

ASTNode::ASTNode()
    : leftMostChild(nullptr),
      leftMostSibling(nullptr), 
      rightSibling(nullptr),
      parent(nullptr),
      nodeType(NodeType::EMPTY),
      nodeValue("")
{
}

ASTNode::ASTNode(NodeType nodeType, std::string nodeValue)
    : leftMostChild(nullptr),
      leftMostSibling(nullptr), 
      rightSibling(nullptr),
      parent(nullptr),
      nodeType(nodeType),
      nodeValue(nodeValue)
{
}

ASTNode::~ASTNode()
{
    delete leftMostChild;
    delete leftMostSibling;
    delete rightSibling;
    delete parent;
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
    return leftMostSibling;
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

std::string ASTNode::getNodeType()
{
    switch (nodeType)
    {
        case NodeType::EMPTY:
            return "EMPTY";
        case NodeType::PROGRAM:
            return "PROGRAM";
        case NodeType::CLASS_LIST:
            return "CLASS_LIST";
        case NodeType::FUNCTION_LIST:
            return "FUNCTION_LIST";
        case NodeType::IMPLEMENTATION_LIST:
            return "IMPLEMENTATION_LIST";
        case NodeType::CLASS:
            return "CLASS";
        case NodeType::FUNCTION:
            return "FUNCTION";
        case NodeType::IMPLEMENTATION:
            return "IMPLEMENTATION";
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
    ysibs->leftMostSibling = xsibs->leftMostSibling;
    ysibs->parent = xsibs->parent;
    
    while (ysibs->rightSibling != nullptr) {
        ysibs = ysibs->rightSibling;
        ysibs->leftMostSibling = xsibs->leftMostSibling;
        ysibs->parent = xsibs->parent;
    }
    
    return ysibs;
}