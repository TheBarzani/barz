#include "AST.h"

void DD(std::string s){
    std::cout << s << std::endl;
}

// Helper function to print contents of a string vector
void printVector(std::vector<ASTNode* >& vec) {
    std::cout << "Vector contents: [";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i]->getNodeType();
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

AST::AST() : root(nullptr) {
    // Initialize empty AST
}

AST::~AST() {
    // Clean up the tree by deleting the root node (which will recursively delete children)
    delete root;
    ASTStack.clear();
}

// Update createNode method to set line number
ASTNode* AST::createNode(NodeType nodeType, std::string nodeValue, int line) {
    ASTNode* node = new ASTNode(nodeType, nodeValue);
    node->setLineNumber(line);
    return node;
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

ASTNode* AST::makeFamily(NodeType op, ASTNode* kid1, ASTNode* kid2) {
    ASTNode* parent = createNode(op, "", kid1->getLineNumber());
    parent->adoptChildren(kid1->makeSiblings(kid2));
    return parent;
}

template<typename... Args>
ASTNode* AST::makeFamily(NodeType op, ASTNode* kid1, ASTNode* kid2, Args... rest) {
    ASTNode* siblings = kid1->makeSiblings(kid2);
    return makeFamily(op, siblings, rest...);
}

ASTNode* AST::makeFamily(NodeType op, ASTNode* kid) {
    ASTNode* parent = createNode(op, "",kid->getLineNumber());
    parent->adoptChildren(kid);
    return parent;
}

ASTNode* AST::getRoot() {
    return root;
}

void AST::performAction(std::string action, std::string value, int line) {
    DD("===========================================================");
    std::cout << "Action: " << action << " Value: " << value << std::endl;
    printVector(ASTStack);

    // Program Structure Actions
    if (action == "_createRoot") {
        this->root = makeFamily(NodeType::PROGRAM,
            createNode(NodeType::CLASS_LIST, "classList", line),
            createNode(NodeType::IMPLEMENTATION_LIST, "implList", line),
            createNode(NodeType::FUNCTION_LIST, "functionList", line)
        );
        this->ASTStack.push_back(this->root);
    }
    else if (action == "_addToProgram") {
        ASTNode* node = ASTStack.back();
        ASTStack.pop_back();
        
        switch (node->getNodeEnum()) {
            case NodeType::CLASS:
                root->getLeftMostChild()->adoptChildren(node);
                break;
            case NodeType::IMPLEMENTATION:
                root->getLeftMostChild()->getRightSibling()->adoptChildren(node);
                break;
            case NodeType::FUNCTION:
                root->getLeftMostChild()->getRightSibling()->getRightSibling()->adoptChildren(node);
                break;
            default:
                break;
        }
        this->root = ASTStack.front();
    }

    // Class-Related Actions
    else if (action == "_createClassId") {
        ASTStack.push_back(createNode(NodeType::CLASS_ID, value, line));
    }
    else if (action == "_createInheritanceList") {
        ASTStack.push_back(createNode(NodeType::INHERITANCE_LIST, "inheritanceList", line));
    }
    else if (action == "_addInheritanceId") {
        ASTNode* inheritId = createNode(NodeType::INHERITANCE_ID, value, line);
        if (!ASTStack.empty()) {
            ASTStack.back()->adoptChildren(inheritId);
        }
    }
    else if (action == "_createClass") {
        ASTNode* memberList = ASTStack.back(); ASTStack.pop_back();
        ASTNode* inheritList = ASTStack.back(); ASTStack.pop_back();
        ASTNode* classId = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* classNode = makeFamily(NodeType::CLASS, classId, inheritList, memberList);
        ASTStack.push_back(classNode);
    }
    else if (action == "_addMemberList") {
        ASTStack.push_back(createNode(NodeType::MEMBER_LIST, "memberList", line));
    }

    // Implementation-Related Actions
    else if (action == "_createImplementationId") {
        ASTStack.push_back(createNode(NodeType::IMPLEMENTATION_ID, value, line));
    }
    else if (action == "_addImplementationFunction") {
        ASTNode* function = ASTStack.back();ASTStack.pop_back();
        ASTStack.back()->adoptChildren(function);
    }
    else if (action == "_createImplementation") {
        ASTNode* functions = ASTStack.back(); ASTStack.pop_back();
        ASTNode* implId = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* impl = makeFamily(NodeType::IMPLEMENTATION, implId, functions);
        ASTStack.push_back(impl);
    }

    // Function-Related Actions
   else if (action == "_createFunctionId") {
        ASTStack.push_back(createNode(NodeType::FUNCTION_ID, value, line));
    }
    else if (action == "_setConstructor") {
        ASTStack.back()->setNodeValue("constructor");
    }
    else if (action == "_createFunctionSignature") {
        ASTNode* returnType = ASTStack.back(); ASTStack.pop_back();
        ASTNode* params = ASTStack.back(); ASTStack.pop_back();
        ASTNode* funcId = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* signature = makeFamily(NodeType::FUNCTION_SIGNATURE, funcId, params, returnType);
        ASTStack.push_back(signature);
    }
    else if (action == "_createConstructorSignature") {
        ASTNode* params = ASTStack.back(); ASTStack.pop_back();
        ASTNode* funcId = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* signature = makeFamily(NodeType::CONSTRUCTOR_SIGNATURE, funcId, params);
        ASTStack.push_back(signature);
    }
    else if (action == "_createFunctionBody") {
        ASTStack.back()->setNodeType(NodeType::FUNCTION_BODY);
        ASTStack.back()->setNodeValue("");
    }
    else if (action == "_createFunction") {
        ASTNode* body = ASTStack.back(); ASTStack.pop_back();
        ASTNode* signature = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* function = makeFamily(NodeType::FUNCTION, signature, body);
        ASTStack.push_back(function);
    }

    // Member-Related Actions
    else if (action == "_setVisibility") {
        ASTStack.push_back(createNode(NodeType::VISIBILITY, value, line));
    }
    else if (action == "_addMember") {
        ASTNode* member = ASTStack.back(); ASTStack.pop_back();
        ASTNode* visibility = ASTStack.back(); ASTStack.pop_back();

        // Check if the stack has the class ID
        if (!ASTStack.empty()) {
            ASTNode* classNode = ASTStack.back();
            ASTNode* memberWithVisibility = makeFamily(NodeType::MEMBER, visibility, member);
            classNode->adoptChildren(memberWithVisibility);
        } else {
            std::cerr << "Error: Class ID not found on stack for _addMember action." << std::endl;
        }
    }

    // Variable and Type Actions
    else if (action == "_setVariableId") {
        ASTStack.push_back(createNode(NodeType::VARIABLE_ID, value, line));
    }
    else if (action == "_setVariableType") {
        ASTStack.push_back(createNode(NodeType::TYPE, value, line));
    }
    else if (action == "_createVariable") {
        ASTNode* type = ASTStack.back(); ASTStack.pop_back();
        ASTNode* id = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* variable = makeFamily(NodeType::VARIABLE, id, type);
        ASTStack.push_back(variable);
    }
    else if (action == "_createLocalVariable") {
        ASTNode* variable = ASTStack.back();
        variable->setNodeType(NodeType::LOCAL_VARIABLE);
    }

    // Statement Actions
    else if (action == "_createBlock") {
        ASTNode* statements = ASTStack.back(); ASTStack.pop_back();
        ASTNode* block = makeFamily(NodeType::BLOCK, statements);
        ASTStack.push_back(block);
    }
    else if (action == "_createIfStatement") {
        ASTNode* elsePart = ASTStack.back(); ASTStack.pop_back();
        ASTNode* thenPart = ASTStack.back(); ASTStack.pop_back();
        ASTNode* condition = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* ifStmt = makeFamily(NodeType::IF_STATEMENT, condition, thenPart, elsePart);
        ASTStack.push_back(ifStmt);
    }
    else if (action == "_createWhileStatement") {
        ASTNode* body = ASTStack.back(); ASTStack.pop_back();
        ASTNode* condition = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* whileStmt = makeFamily(NodeType::WHILE_STATEMENT, condition, body);
        ASTStack.push_back(whileStmt);
    }

    // Expression Actions
    else if (action == "_createRelationalExpr") {
        ASTNode* right = ASTStack.back(); ASTStack.pop_back();
        ASTNode* op = ASTStack.back(); ASTStack.pop_back();
        ASTNode* left = ASTStack.back(); ASTStack.pop_back();
        op->adoptChildren(left);
        op->adoptChildren(right);
        ASTNode* relExpr = makeFamily(NodeType::RELATIONAL_EXPR, op);
        ASTStack.push_back(relExpr);
    }
    else if (action == "_createAssignment") {
        ASTNode* expr = ASTStack.back(); ASTStack.pop_back();
        ASTNode* op = ASTStack.back(); ASTStack.pop_back();
        ASTNode* var = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* assign = makeFamily(NodeType::ASSIGNMENT, var, expr);
        ASTStack.push_back(assign);
        delete op;
    }
    else if (action == "_createFunctionDeclaration") {
        ASTNode* signature = ASTStack.back(); ASTStack.pop_back();
        ASTNode* functionDeclaration = makeFamily(NodeType::FUNCTION_DECLARATION, signature);
        ASTStack.push_back(functionDeclaration);
    }
    else if (action == "_createAttribute") {
        ASTNode* variable = ASTStack.back(); ASTStack.pop_back();
        ASTNode* attribute = makeFamily(NodeType::ATTRIBUTE, variable);
        ASTStack.push_back(attribute);
    }
    else if (action == "_createSingleStatement") {
        ASTNode* statement = ASTStack.back(); ASTStack.pop_back();
        ASTNode* singleStatement = makeFamily(NodeType::SINGLE_STATEMENT, statement);
        ASTStack.push_back(singleStatement);
    }
    else if (action == "_addStatement") {
        ASTNode* statement = ASTStack.back(); ASTStack.pop_back();
        ASTNode* statements = ASTStack.back();
        statements->adoptChildren(statement);
    }
    else if (action == "_createExpressionStatement") {
        // For handling expression statements
        //  ASTNode* expression = ASTStack.back(); ASTStack.pop_back();
        //  ASTNode* expressionStatement = makeFamily(NodeType::EXPRESSION_STATEMENT, expression);
        //  ASTStack.push_back(expressionStatement);
    }
    else if (action == "_createReadStatement") {
         ASTNode* variable = ASTStack.back(); ASTStack.pop_back();
         ASTNode* readStatement = makeFamily(NodeType::READ_STATEMENT, variable);
         ASTStack.push_back(readStatement);
    }
   else if (action == "_createWriteStatement") {
        ASTNode* expr = ASTStack.back(); ASTStack.pop_back();
        ASTNode* writeStatement = makeFamily(NodeType::WRITE_STATEMENT, expr);
        ASTStack.push_back(writeStatement);
    }
    else if (action == "_createReturnStatement") {
            ASTNode* expr = ASTStack.back(); ASTStack.pop_back();
            ASTNode* returnStatement = makeFamily(NodeType::RETURN_STATEMENT, expr);
            ASTStack.push_back(returnStatement);
    }
    else if (action == "_setAssignOperator") {
            ASTNode* assignOp = createNode(NodeType::ASSIGN_OP, value, line);
            ASTStack.push_back(assignOp);
    }
    else if (action == "_setRelop") {
            ASTNode* relop = createNode(NodeType::REL_OP, value, line);
            ASTStack.push_back(relop);
    }
    else if (action == "_setAddOp") {
            ASTNode* addop = createNode(NodeType::ADD_OP, value, line);
            ASTStack.push_back(addop);
    }
    else if (action == "_setMultOp") {
            ASTNode* multop = createNode(NodeType::MULT_OP, value, line);
            ASTStack.push_back(multop);
    }
    else if (action == "_setIdentifier") {
            ASTNode* identifier = createNode(NodeType::IDENTIFIER, value, line);
            ASTStack.push_back(identifier);
    }
    else if (action == "_setSelfIdentifier") {
            ASTNode* selfIdentifier = createNode(NodeType::SELF_IDENTIFIER, "self", line);
            ASTStack.push_back(selfIdentifier);
    }
    else if (action == "_setTypeInt" || action == "_setTypeFloat" || action == "_setTypeCustom" || action == "_setTypeVoid") {
            ASTNode* typeNode = createNode(NodeType::TYPE, value, line);
            ASTStack.push_back(typeNode);
    }
    else if (action == "_addArrayDimension") {
            ASTNode* arrayDimension = createNode(NodeType::ARRAY_DIMENSION, value, line);
            ASTStack.push_back(arrayDimension);
    }
    else if (action == "_addDynamicArrayDimension") {
            ASTNode* dynamicArrayDimension = createNode(NodeType::ARRAY_DIMENSION, "dynamic", line);
            ASTStack.push_back(dynamicArrayDimension);
    }
    else if (action == "_createParamList") {
            ASTStack.push_back(createNode(NodeType::PARAM_LIST, "paramList", line));
    }
    else if (action == "_createParam") {
            ASTNode* param;
            if(ASTStack.back()->getNodeEnum() == NodeType::ARRAY_DIMENSION){
                ASTNode* array_dimension = ASTStack.back(); ASTStack.pop_back();
                ASTNode* type = ASTStack.back(); ASTStack.pop_back();
                ASTNode* id = ASTStack.back(); ASTStack.pop_back();
                param = makeFamily(NodeType::PARAM, id, type, array_dimension);
            }
            else{
                ASTNode* type = ASTStack.back(); ASTStack.pop_back();
                ASTNode* id = ASTStack.back(); ASTStack.pop_back();
                param = makeFamily(NodeType::PARAM, id, type);
            }

            // Get the parameter list node
            ASTNode* paramList = ASTStack.back();

            // Add the formal parameter to the list
            paramList->adoptChildren(param);
    }
    else if (action == "_setParamId"){
            ASTStack.push_back(createNode(NodeType::PARAM_ID, value, line));
    }
    else if (action == "_addActualParam") {
            ASTNode* expr = ASTStack.back(); ASTStack.pop_back();
            ASTNode* actualParamsList = ASTStack.back();
            actualParamsList->adoptChildren(expr);
    }
    else if (action == "_createFunctionCall") {
            ASTNode* params = ASTStack.back(); ASTStack.pop_back();
            ASTNode* id = ASTStack.back(); ASTStack.pop_back();
            ASTNode* functionCall = makeFamily(NodeType::FUNCTION_CALL, id, params);
            ASTStack.push_back(functionCall);
    }
    else if (action == "_processArrayAccess") {
            ASTNode* indices = ASTStack.back(); ASTStack.pop_back();
            ASTNode* variable = ASTStack.back(); ASTStack.pop_back();
            ASTNode* arrayAccess = makeFamily(NodeType::ARRAY_ACCESS, variable, indices);
            ASTStack.push_back(arrayAccess);
    }
    else if (action == "_pushIdentifier") {
            ASTNode* identifier = createNode(NodeType::IDENTIFIER, value, line);
            ASTStack.push_back(identifier);
    }
    else if (action == "_pushDotIdentifier") {
            ASTNode* dotIdentifier = createNode(NodeType::DOT_IDENTIFIER, value, line);
            ASTStack.push_back(dotIdentifier);
    }
    else if (action ==  "_finishVariable") {
            // For handling the completion of a variable node
            ASTNode* identifier = ASTStack.back(); ASTStack.pop_back();
            ASTNode* variableNode = makeFamily(NodeType::VARIABLE, identifier);
            ASTStack.push_back(variableNode);
    }
    else if (action == "_finishFactor") {
            // For handling the completion of a factor node
            // ASTNode* identifier = ASTStack.back(); ASTStack.pop_back();
            // ASTNode* factorNode = makeFamily(NodeType::FACTOR, identifier);
            // ASTStack.push_back(factorNode);
    }
    else if (action == "_addTerm") {
        // // For handling the completion of a term node
        // ASTNode* factor = ASTStack.back(); ASTStack.pop_back();
        
        // // If factor is a literal or operator, don't wrap it
        // if (factor->getNodeEnum() == NodeType::INT || 
        //     factor->getNodeEnum() == NodeType::FLOAT || 
        //     factor->getNodeEnum() == NodeType::ADD_OP || 
        //     factor->getNodeEnum() == NodeType::MULT_OP) {
        //     ASTStack.push_back(factor);
        // } else {
        //     ASTNode* termNode = makeFamily(NodeType::TERM, factor);
        //     ASTStack.push_back(termNode);
        // }
    }
    else if (action == "_finishArithExpr") {
        // // For handling the completion of an arithmetic expression node
        // ASTNode* term = ASTStack.back(); ASTStack.pop_back();
        
        // // If term is a literal or operator, don't wrap it
        // if (term->getNodeEnum() == NodeType::INT || 
        //     term->getNodeEnum() == NodeType::FLOAT || 
        //     term->getNodeEnum() == NodeType::ADD_OP || 
        //     term->getNodeEnum() == NodeType::MULT_OP) {
        //     ASTStack.push_back(term);
        // } else {
        //     ASTNode* arithExprNode = makeFamily(NodeType::ARITH_EXPR, term);
        //     ASTStack.push_back(arithExprNode);
        // }
    }
    else if (action == "_finishExpression") {
        // // For handling the completion of an expression node
        // ASTNode* arithExpr = ASTStack.back(); ASTStack.pop_back();
        
        // // If arithExpr is a literal or operator, don't wrap it
        // if (arithExpr->getNodeEnum() == NodeType::INT || 
        //     arithExpr->getNodeEnum() == NodeType::FLOAT || 
        //     arithExpr->getNodeEnum() == NodeType::ADD_OP || 
        //     arithExpr->getNodeEnum() == NodeType::MULT_OP) {
        //     ASTStack.push_back(arithExpr);
        // } else {
        //     ASTNode* exprNode = makeFamily(NodeType::EXPR, arithExpr);
        //     ASTStack.push_back(exprNode);
        // }
    }
    else if (action == "_processDotAccess") {
        if (ASTStack.back()->getNodeEnum() != NodeType::DOT_IDENTIFIER && ASTStack.back()->getNodeEnum() != NodeType::FUNCTION_CALL) {
            return;
        }
        ASTNode* identifier = ASTStack.back(); ASTStack.pop_back();// Create DOT_IDENTIFIER node
        ASTNode* left = ASTStack.back(); ASTStack.pop_back(); // Get the left-hand side

        // Create a new node to represent the dot access (e.g., DOT_ACCESS)
        ASTNode* dotAccess = makeFamily(NodeType::DOT_ACCESS, left, identifier);

        ASTStack.push_back(dotAccess); // Push the result onto the stack
    }
    else if (action == "_addBodyStatement") {
        ASTNode* statement = ASTStack.back(); ASTStack.pop_back(); // Get the statement
    
        ASTStack.back()->adoptChildren(statement); // Add the statement to the block
    }
    else if (action == "_addStatementsList"){
        ASTStack.push_back(createNode(NodeType::STATEMENTS_LIST, "", line));
    }
    else if (action == "_createImplementationList"){
        ASTStack.push_back(createNode(NodeType::IMPLEMENTATION_FUNCTION_LIST,"", line));
    }
    else if (action == "_processMultOp"){
        ASTNode* right = ASTStack.back(); ASTStack.pop_back();
        ASTNode* op = ASTStack.back(); ASTStack.pop_back();
        ASTNode* left = ASTStack.back(); ASTStack.pop_back();
        
        // Directly connect operands to the operator node  
        op->adoptChildren(left);
        op->adoptChildren(right);
        
        // Don't wrap in TERM, push the op node directly
        ASTStack.push_back(op);
    }
    else if (action == "_processAddOp"){
        ASTNode* right = ASTStack.back(); ASTStack.pop_back();
        ASTNode* op = ASTStack.back(); ASTStack.pop_back();
        ASTNode* left = ASTStack.back(); ASTStack.pop_back();
        
        // Directly connect operands to the operator node
        op->adoptChildren(left);
        op->adoptChildren(right);
        
        // Don't wrap in TERM, push the op node directly
        ASTStack.push_back(op);
    }
    else if (action == "_addCondition"){
        ASTNode* condition = ASTStack.back(); ASTStack.pop_back();
        ASTStack.push_back(makeFamily(NodeType::CONDITION, condition));
    }
    else if(action == "_addEmptyBlock"){
        ASTStack.push_back(createNode(NodeType::BLOCK, "empty", line));
    }
    else if(action == "_processArraySize"){
        ASTNode * type = ASTStack.back(); ASTStack.pop_back();
        ASTNode * dimension = ASTStack.back(); ASTStack.pop_back();
        ASTStack.push_back(makeFamily(NodeType::ARRAY_TYPE, type, dimension));
    }
    else if (action == "_pushIntLiteral") {
        ASTNode* intNode = createNode(NodeType::INT, value, line);
        ASTStack.push_back(intNode);
    }
    else if (action == "_pushFloatLiteral") {
        ASTNode* floatNode = createNode(NodeType::FLOAT, value, line);
        ASTStack.push_back(floatNode);
    }
    // Debug output
    printVector(ASTStack);
    std::cout << "Stack size after action: " << ASTStack.size() << std::endl;
}