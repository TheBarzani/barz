#include "AST.h"

void DD(std::string s){
    std::cout << s << std::endl;
}

// Helper function to print contents of a string vector
void printVector(const std::vector<ASTNode* >& vec) {
    std::cout << "Vector contents: [";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i]->getNodeType();
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

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
                << node->getNodeType() << " | \\"
                << node->getNodeValue() << " \"];\n";
            
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

void AST::performAction(std::string action, std::string value) {
    if (action == "_createLists") {
        // Create the lists by pushing ASTnodes onto the stack
        ASTNode *classList = createNode(NodeType::CLASS_LIST, "");
        ASTNode *funcList = createNode(NodeType::FUNCTION_LIST, "");
        ASTNode *implList = createNode(NodeType::IMPLEMENTATION_LIST, "");
        
        // Create the root node
        this->root = createNode(NodeType::PROGRAM, "");

        // Merge the lists
        this->root->adoptChildren(classList);
        this->root->adoptChildren(funcList);
        this->root->adoptChildren(implList);
        this->ASTStack.push_back(this->root);
    }
    else if (action == "_createClass") { 
        if (ASTStack.back()->getNodeEnum() == NodeType::PROGRAM){
            return;
        }
        current = ASTStack.back(); ASTStack.pop_back();
        while (ASTStack.back()->getNodeEnum() != NodeType::PROGRAM){
            ASTStack.back()->makeSiblings(current); current = ASTStack.back(); ASTStack.pop_back();
        }
        ASTNode * classNode = createNode(NodeType::CLASS, "");
        classNode->adoptChildren(current);
        (ASTStack.back())->getLeftMostChild()->adoptChildren(classNode);
    }
    else if (action == "_createFunction") {
        current = createNode(NodeType::FUNCTION, "");
        (ASTStack.back())->getLeftMostChild()->getRightSibling()->adoptChildren(current);
    }
    else if (action == "_createImplementation") {
        current = createNode(NodeType::IMPLEMENTATION, "");
        (ASTStack.back())->getLeftMostChild()->getRightSibling()->getRightSibling()->adoptChildren(current);
    }
    else if (action == "_createClassId") {
        current = createNode(NodeType::CLASS_ID, value);
        ASTStack.push_back(current);
    }
    else if (action == "_createInheritanceList"){
        current = createNode(NodeType::INHERITANCE_LIST, "");
        ASTStack.push_back(current);   
    }
    else if (action == "_addInheritanceId"){
        current = createNode(NodeType::INHERITANCE_ID, value);
        ASTStack.push_back(current);
    }
    else if (action == "_makeInheritanceList"){
        if (ASTStack.back()->getNodeEnum() == NodeType::INHERITANCE_LIST){
            return;
        }
        current = ASTStack.back(); ASTStack.pop_back();
        while (ASTStack.back()->getNodeEnum() != NodeType::INHERITANCE_LIST){
            current->setLeftMostSibling(ASTStack.back()); ASTStack.pop_back();
        }
        ASTStack.back()->adoptChildren(current);
    }
    else if (action == "_setVisibility"){
        current = createNode(NodeType::VISIBILITY, value);
        ASTStack.push_back(current);
    }
    else if (action == "_createMemberList"){
        current = createNode(NodeType::MEMBER_LIST, "");
        ASTStack.push_back(current);
    }
    else if (action == "_addMember"){; 
        current = createNode(NodeType::MEMBER, "");
        ASTStack.push_back(current);
    }
    else if (action == "_makeMemberList"){
        if (ASTStack.back()->getNodeEnum() == NodeType::MEMBER_LIST){
            return;
        }
        current = ASTStack.back(); ASTStack.pop_back();
        while (ASTStack.back()->getNodeEnum() != NodeType::MEMBER && ASTStack.back()->getNodeEnum() != NodeType::MEMBER){
            DD(ASTStack.back()->getNodeType());
            ASTStack.back()->makeSiblings(current); current = ASTStack.back(); ASTStack.pop_back();
        }
        printVector(ASTStack); 
        ASTStack.back()->adoptChildren(current); current = ASTStack.back(); ASTStack.pop_back();
        (ASTStack.back())->getLeftMostChild()->adoptChildren(current);
    }
    else if (action == "_addVariable"){
        current = createNode(NodeType::VARIABLE, "");
        ASTStack.push_back(current);
    }
    else if (action == ""){
        
    }

}

