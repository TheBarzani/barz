#include <AST.h>

AST::AST() : root(nullptr), current(nullptr) {
    // Initialize empty AST
}

AST::~AST() {
    // Clean up the tree by deleting the root node (which will recursively delete children)
    if (root != nullptr) {
        delete root;
        root = nullptr;
    }
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
    // TODO: Implement AST output to file in DOT language format
    outFile.close();
}

void AST::performAction(std::string action) {
    if (action == "createLists")
    {

    }
    else if (action == "mergeLists")
    {

    }
    else if (action == "createClass")
    {
    
    }
    else if (action == "createFunc")
    {

    }
    else if (action == "createImpl")
    {

    }
    else if (action == "addToList")
    {

    }
    
}