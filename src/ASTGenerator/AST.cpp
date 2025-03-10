#include "AST.h"

AST::AST() : root(nullptr), current(nullptr) {
    // Initialize empty AST
}

AST::~AST() {
    // Clean up the tree by deleting the root node (which will recursively delete children)
    delete root;
    delete current;
    ASTStack.clear();
}

ASTNode* AST::createNode(NodeType nodeType, std::string nodeValue) {
    return new ASTNode(nodeType, nodeValue);
}

void AST::writeToFile(std::string filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }
    
    // Write DOT file header
    outFile << "digraph AST {\n";
    outFile << "  node [shape=record];\n";
    outFile << "    node [fontname=Sans];charset=\"UTF-8\" splines=true splines=spline rankdir =LR\n";
    
    if (root != nullptr) {
        // Lambda for creating nodes
        auto createNodes = [](ASTNode* node, std::ofstream& out, auto& self) -> void {
            if (node == nullptr) return;
            
            // Create this node
            out << "  node" << node->getNodeNumber() << " [label=\""
                << node->getNodeType() << " | "
                << node->getNodeValue() << "\"];\n";
            
            // Process left child and right siblings
            self(node->getLeftMostChild(), out, self);
            self(node->getRightSibling(), out, self);
        };

        // Lambda for creating edges
        auto createEdges = [](ASTNode* node, std::ofstream& out, auto& self) -> void {
            if (node == nullptr) return;
            
            // Create edges to children
            ASTNode* child = node->getLeftMostChild();
            while (child != nullptr) {
                out << "  node" << node->getNodeNumber() << " -> node" 
                    << child->getNodeNumber() << ";\n";
                
                // Process child's subtree
                self(child, out, self);
                
                // Move to next sibling
                child = child->getRightSibling();
            }
        };
        
        // Apply the lambdas
        createNodes(root, outFile, createNodes);
        createEdges(root, outFile, createEdges);
    }
    std::cout << "Writing to file" << std::endl;
    outFile << "}\n";
    outFile.close();
}

void AST::performAction(std::string action) {
    if (action == "_createLists")
    {
        // Create the lists by pushing ASTnodes onto the stack
        ASTNode *classList = createNode(NodeType::CLASS_LIST, "");
        ASTNode *funcList = createNode(NodeType::FUNCTION_LIST, "");
        ASTNode *implList = createNode(NodeType::IMPLEMENTATION_LIST, "");
        
        // Create the root node
        this->root = createNode(NodeType::PROGRAM, "");
        this->root->setParent(nullptr);
        this->root->setLeftMostSibling(nullptr);
        this->root->setRightSibling(nullptr);

        // Merge the lists
        this->root->adoptChildren(classList);
        this->root->adoptChildren(funcList);
        this->root->adoptChildren(implList);
        this->ASTStack.push_back(this->root);
    }
    else if (action == "_createClass")
    {   
        std::cout << "Creating class==========================================" << std::endl;
        // TODO: Implement class creation
        current = createNode(NodeType::CLASS, "");
        // std::cout << ASTStack.back()->getLeftMostChild()->getNodeType() << std::endl;
        (ASTStack.back())->getLeftMostChild()->adoptChildren(current);
    }
    else if (action == "_createFunction")
    {
        // TODO: Implement function creation
        current = createNode(NodeType::FUNCTION, "");
        (ASTStack.back())->getLeftMostChild()->getRightSibling()->adoptChildren(current);
    }
    else if (action == "_createImplemepntation")
    {
        // TODO: Implement implementation creation
        current = createNode(NodeType::IMPLEMENTATION, "");
        current->setParent((ASTStack.back())->getLeftMostChild()->getRightSibling()->getRightSibling()); // Add to function list
        (ASTStack.back())->getLeftMostChild()->getRightSibling()->getRightSibling()->adoptChildren(current);
    }
    
}