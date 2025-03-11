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

AST::AST() : root(nullptr) {
    // Initialize empty AST
}

AST::~AST() {
    // Clean up the tree by deleting the root node (which will recursively delete children)
    delete root;
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

ASTNode* AST::makeFamily(NodeType op, ASTNode* kid1, ASTNode* kid2) {
    ASTNode* parent = createNode(op, "");
    parent->adoptChildren(kid1->makeSiblings(kid2));
    return parent;
}

template<typename... Args>
ASTNode* AST::makeFamily(NodeType op, ASTNode* kid1, ASTNode* kid2, Args... rest) {
    ASTNode* siblings = kid1->makeSiblings(kid2);
    return makeFamily(op, siblings, rest...);
}

ASTNode* AST::makeFamily(NodeType op, ASTNode* kid) {
    ASTNode* parent = createNode(op, "");
    parent->adoptChildren(kid);
    return parent;
}

void AST::performAction(std::string action, std::string value) {
    std::cout << "Action: " << action << " Value: " << value << std::endl;
    printVector(ASTStack);

    // Program Structure Actions
    if (action == "_createRoot") {
        root = makeFamily(NodeType::PROGRAM,
            createNode(NodeType::CLASS_LIST, "classList"),
            createNode(NodeType::FUNCTION_LIST, "functionList"),
            createNode(NodeType::IMPLEMENTATION_LIST, "implList")
        );
        ASTStack.push_back(root);
    }
    else if (action == "_addToProgram") {
        ASTNode* node = ASTStack.back();
        ASTStack.pop_back();
        
        switch (node->getNodeEnum()) {
            case NodeType::CLASS:
                root->getLeftMostChild()->adoptChildren(node);
                break;
            case NodeType::FUNCTION:
                root->getLeftMostChild()->getRightSibling()->adoptChildren(node);
                break;
            case NodeType::IMPLEMENTATION:
                root->getLeftMostChild()->getRightSibling()->getRightSibling()->adoptChildren(node);
                break;
            default:
                break;
        }
    }

    // Class-Related Actions
    else if (action == "_createClassId") {
        ASTStack.push_back(createNode(NodeType::CLASS_ID, value));
    }
    else if (action == "_createInheritanceList") {
        ASTStack.push_back(createNode(NodeType::INHERITANCE_LIST, "inheritanceList"));
    }
    else if (action == "_addInheritanceId") {
        ASTNode* inheritId = createNode(NodeType::INHERITANCE_ID, value);
        if (!ASTStack.empty()) {
            ASTStack.back()->adoptChildren(inheritId);
        }
    }
    else if (action == "_makeInheritanceList") {
        // The inheritance list is already built through _addInheritanceId
    }
    else if (action == "_createClass") {
        ASTNode* memberList = ASTStack.back(); ASTStack.pop_back();
        ASTNode* inheritList = ASTStack.back(); ASTStack.pop_back();
        ASTNode* classId = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* classNode = makeFamily(NodeType::CLASS, classId, inheritList, memberList);
        ASTStack.push_back(classNode);
    }
    else if (action == "_addMemberList") {
        ASTStack.push_back(createNode(NodeType::MEMBER_LIST, "memberList"));
    }

    // Implementation-Related Actions
    else if (action == "_createImplementationId") {
        ASTStack.push_back(createNode(NodeType::IMPLEMENTATION_ID, value));
    }
    else if (action == "_addImplementationFunction") {
        ASTNode* function = ASTStack.back();
        ASTNode* impl = ASTStack[ASTStack.size() - 2];
        impl->adoptChildren(function);
    }
    else if (action == "_createImplementation") {
        ASTNode* functions = ASTStack.back(); ASTStack.pop_back();
        ASTNode* implId = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* impl = makeFamily(NodeType::IMPLEMENTATION, implId, functions);
        ASTStack.push_back(impl);
    }

    // Function-Related Actions
   else if (action == "_createFunctionId") {
        ASTStack.push_back(createNode(NodeType::FUNCTION_ID, value));
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
        ASTNode* statements = ASTStack.back(); ASTStack.pop_back();
        ASTNode* body = makeFamily(NodeType::FUNCTION_BODY, statements);
        ASTStack.push_back(body);
    }
    else if (action == "_createFunction") {
        ASTNode* body = ASTStack.back(); ASTStack.pop_back();
        ASTNode* signature = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* function = makeFamily(NodeType::FUNCTION, signature, body);
        ASTStack.push_back(function);
    }

    // Member-Related Actions
    else if (action == "_setVisibility") {
        ASTStack.push_back(createNode(NodeType::VISIBILITY, value));
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
        ASTStack.push_back(createNode(NodeType::VARIABLE_ID, value));
    }
    else if (action == "_setVariableType") {
        ASTStack.push_back(createNode(NodeType::TYPE, value));
    }
    else if (action == "_createVariable") {
        ASTNode* type = ASTStack.back(); ASTStack.pop_back();
        ASTNode* id = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* variable = makeFamily(NodeType::VARIABLE, id, type);
        ASTStack.push_back(variable);
    }
    else if (action == "_createLocalVariable") {
        ASTNode* variable = ASTStack.back(); ASTStack.pop_back();
        ASTNode* localVar = makeFamily(NodeType::LOCAL_VARIABLE, variable);
        ASTStack.push_back(localVar);
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
        
        ASTNode* relExpr = makeFamily(NodeType::RELATIONAL_EXPR, left, op, right);
        ASTStack.push_back(relExpr);
    }
    else if (action == "_createAssignment") {
        ASTNode* expr = ASTStack.back(); ASTStack.pop_back();
        ASTNode* op = ASTStack.back(); ASTStack.pop_back();
        ASTNode* var = ASTStack.back(); ASTStack.pop_back();
        
        ASTNode* assign = makeFamily(NodeType::ASSIGNMENT, var, op, expr);
        ASTStack.push_back(assign);
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
         ASTNode* expression = ASTStack.back(); ASTStack.pop_back();
         ASTNode* expressionStatement = makeFamily(NodeType::EXPRESSION_STATEMENT, expression);
         ASTStack.push_back(expressionStatement);
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
        ASTNode* assignOp = createNode(NodeType::ASSIGN_OP, value);
        ASTStack.push_back(assignOp);
   }
   else if (action == "_setRelop") {
        ASTNode* relop = createNode(NodeType::REL_OP, value);
        ASTStack.push_back(relop);
   }
   else if (action == "_setAddOp") {
        ASTNode* addop = createNode(NodeType::ADD_OP, value);
        ASTStack.push_back(addop);
   }
   else if (action == "_setMultOp") {
        ASTNode* multop = createNode(NodeType::MULT_OP, value);
        ASTStack.push_back(multop);
   }
   else if (action == "_setIdentifier") {
        ASTNode* identifier = createNode(NodeType::IDENTIFIER, value);
        ASTStack.push_back(identifier);
   }
   else if (action == "_setSelfIdentifier") {
        ASTNode* selfIdentifier = createNode(NodeType::SELF_IDENTIFIER, "self");
        ASTStack.push_back(selfIdentifier);
   }
   else if (action == "_setTypeInt" || action == "_setTypeFloat" || action == "_setTypeCustom" || action == "_setTypeVoid") {
        ASTNode* typeNode = createNode(NodeType::TYPE, value);
        ASTStack.push_back(typeNode);
   }
   else if (action == "_addArrayDimension") {
        ASTNode* arrayDimension = createNode(NodeType::ARRAY_DIMENSION, value);
        ASTStack.push_back(arrayDimension);
   }
   else if (action == "_addDynamicArrayDimension") {
        ASTNode* dynamicArrayDimension = createNode(NodeType::DYNAMIC_ARRAY_DIMENSION, "[]");
        ASTStack.push_back(dynamicArrayDimension);
   }
   else if (action == "_createParamList") {
        ASTStack.push_back(createNode(NodeType::PARAM_LIST, "paramList"));
   }
   else if (action == "_createFormalParam") {
        ASTNode* type = ASTStack.back(); ASTStack.pop_back();
        ASTNode* id = ASTStack.back(); ASTStack.pop_back();
        ASTNode* formalParam = makeFamily(NodeType::FORMAL_PARAM, id, type);

        // Get the parameter list node
        ASTNode* paramList = ASTStack.back();

        // Add the formal parameter to the list
        paramList->adoptChildren(formalParam);

        // Push the parameter list back onto the stack
        ASTStack.push_back(paramList);
   }
   else if (action == "_addFormalParam") {
        ASTNode* formalParam = ASTStack.back(); ASTStack.pop_back();
        ASTNode* formalParams = ASTStack.back();
        formalParams->adoptChildren(formalParam);
   }
   else if (action == "_addActualParam") {
        ASTNode* expr = ASTStack.back(); ASTStack.pop_back();
        ASTNode* actualParams = ASTStack.back();
        actualParams->adoptChildren(expr);
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
        ASTNode* identifier = createNode(NodeType::IDENTIFIER, value);
        ASTStack.push_back(identifier);
   }
   else if (action == "_pushDotIdentifier") {
        ASTNode* dotIdentifier = createNode(NodeType::DOT_IDENTIFIER, value);
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
        ASTNode* identifier = ASTStack.back(); ASTStack.pop_back();
        ASTNode* factorNode = makeFamily(NodeType::FACTOR, identifier);
        ASTStack.push_back(factorNode);
   }
   else if (action == "_finishTerm") {
        // For handling the completion of a term node
        ASTNode* factor = ASTStack.back(); ASTStack.pop_back();
        ASTNode* termNode = makeFamily(NodeType::TERM, factor);
        ASTStack.push_back(termNode);
   }
   else if (action == "_finishArithExpr") {
        // For handling the completion of an arithmetic expression node
        ASTNode* term = ASTStack.back(); ASTStack.pop_back();
        ASTNode* arithExprNode = makeFamily(NodeType::ARITH_EXPR, term);
        ASTStack.push_back(arithExprNode);
   }
   else if (action == "_finishExpression") {
        // For handling the completion of an expression node
        ASTNode* arithExpr = ASTStack.back(); ASTStack.pop_back();
        ASTNode* exprNode = makeFamily(NodeType::EXPR, arithExpr);
        ASTStack.push_back(exprNode);
   }
    // Debug output
    std::cout << "Stack size after action: " << ASTStack.size() << std::endl;
}