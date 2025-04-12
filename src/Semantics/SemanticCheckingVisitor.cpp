#include "SemanticCheckingVisitor.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

// Constructor
SemanticCheckingVisitor::SemanticCheckingVisitor(std::shared_ptr<SymbolTable> globalTable)
    : globalTable(globalTable), currentTable(globalTable) {
    currentClassName = "";
    currentFunctionName = "";
    currentType = "";
}

// Destructor
SemanticCheckingVisitor::~SemanticCheckingVisitor() {
}

// Program visitor - starts the semantic analysis
void SemanticCheckingVisitor::visitProgram(ASTNode* node) {
    // Start at global scope
    currentTable = globalTable;
    currentClassName = "";
    
    // Visit all children to collect information
    ASTNode* currentChild = node->getLeftMostChild();
    while (currentChild) {
        currentChild->accept(this);
        currentChild = currentChild->getRightSibling();
    }
    
    // After visiting all nodes, perform additional semantic checks
    
    // 1. Check for circular class dependencies
    for (const auto& [className, _] : globalTable->getNestedTables()) {
        auto classSymbol = globalTable->lookupSymbol(className);
        if (classSymbol && classSymbol->getKind() == SymbolKind::CLASS) {
            std::unordered_set<std::string> visited;
            if (checkClassCircularDependency(className, visited)) {
                reportError("Circular class dependency detected involving class: " + className, node);
            }
        }
    }
    
    // 2. Check for duplicate declarations
    checkDuplicateClassDeclarations();
    checkDuplicateFunctionDeclarations();
    
    // // 3. Check for duplicate members in each class
    // for (const auto& [className, _] : globalTable->getNestedTables()) {
    //     auto classSymbol = globalTable->lookupSymbol(className);
    //     if (classSymbol && classSymbol->getKind() == SymbolKind::CLASS) {
    //         checkDuplicateMemberDeclarations(className);
    //         checkShadowedInheritedMembers(className);
    //     }
    // }
    
    // 4. Check for undefined/undeclared member functions
    // No need this is taken care by the symbol table visit
    // checkUndefinedMemberFunctions();
    // checkUndeclaredMemberFunctions();
}

// Method to check for circular class dependencies
bool SemanticCheckingVisitor::checkClassCircularDependency(const std::string& className, std::unordered_set<std::string>& visited) {
    // If we've already visited this class in this path, we have a cycle
    if (visited.find(className) != visited.end()) {
        return true;
    }
    
    // Mark this class as visited
    visited.insert(className);
    
    // Get the class symbol
    auto classSymbol = globalTable->lookupSymbol(className);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS) {
        visited.erase(className);
        return false; // Not a class
    }
    
    // Get the class's symbol table
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable) {
        visited.erase(className);
        return false; // No table for this class
    }
    
    // Check for circular inheritance
    const auto& inheritedClasses = classSymbol->getInheritedClasses();
    for (const auto& parentClass : inheritedClasses) {
        if (checkClassCircularDependency(parentClass, visited)) {
            return true;
        }
    }
    
    // Check for circular dependencies through data members
    for (const auto& [memberName, memberSymbol] : classTable->getSymbols()) {
        if (memberSymbol->getKind() == SymbolKind::VARIABLE) {
            std::string memberType = memberSymbol->getType();
            // If the member is of a class type, check for circular dependency
            auto memberTypeSymbol = globalTable->lookupSymbol(memberType);
            if (memberTypeSymbol && memberTypeSymbol->getKind() == SymbolKind::CLASS) {
                if (checkClassCircularDependency(memberType, visited)) {
                    return true;
                }
            }
        }
    }
    
    // Remove this class from the visited set since we're done exploring this path
    visited.erase(className);
    
    return false; // No circular dependency found
}

// Assignment visitor - for type checking assignments
void SemanticCheckingVisitor::visitAssignment(ASTNode* node) {
    // Process left side (variable/identifier)
    ASTNode* leftNode = node->getLeftMostChild();
    if (leftNode) {
        leftNode->accept(this);
        TypeInfo leftType = currentExprType;
        
        // Process right side (expression)
        ASTNode* rightNode = leftNode->getRightSibling();
        if (rightNode) {
            rightNode->accept(this);
            TypeInfo rightType = currentExprType;
            
            // Check type compatibility
            if (!areTypesCompatible(leftType, rightType)) {
                reportError("Type mismatch in assignment. Left side is " + formatTypeInfo(leftType) + 
                            " but right side is " + formatTypeInfo(rightType), node);
            }
            
            // Special case for int -> float coercion - emit a warning instead of error
            if (leftType.type == "float" && rightType.type == "int") {
                reportWarning("Implicit conversion from int to float in assignment", node);
            }
        }
    }
}

// Function call visitor - for checking function calls
void SemanticCheckingVisitor::visitFunctionCall(ASTNode* node) {
    // Get function name
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (!funcIdNode) {
        reportError("Function call missing identifier.", node);
        return;
    }
    
    std::string funcName = funcIdNode->getNodeValue();
    
    // Case-insensitive function lookup
    auto funcSymbol = lookupFunctionCaseInsensitive(funcName);
    
    if (!funcSymbol) {
        reportError("Use of undeclared free function: " + funcName, node);
        currentExprType.type = "error";
        return;
    }
    
    // Gather parameter types
    std::vector<TypeInfo> paramTypes;
    ASTNode* argsNode = funcIdNode->getRightSibling();
    if (argsNode) {
        ASTNode* arg = argsNode->getLeftMostChild();
        while (arg) {
            arg->accept(this);
            paramTypes.push_back(currentExprType);
            arg = arg->getRightSibling();
        }
    }
    
    // Check if in class context
    if (!currentClassName.empty()) {
        // Look for member function first
        auto classTable = globalTable->getNestedTable(currentClassName);
        if (classTable) {
            auto funcSymbol = classTable->lookupSymbol(funcName);
            if (funcSymbol && funcSymbol->getKind() == SymbolKind::FUNCTION) {
                // Check parameter count
                const auto& declaredParams = funcSymbol->getParams();
                if (declaredParams.size() != paramTypes.size()) {
                    reportError("Function " + currentClassName + "::" + funcName + 
                               " called with wrong number of arguments. Expected " + 
                               std::to_string(declaredParams.size()) + ", got " + 
                               std::to_string(paramTypes.size()), node);
                } else {
                    // Check each parameter type
                    for (size_t i = 0; i < declaredParams.size(); ++i) {
                        TypeInfo declaredType = parseTypeString(declaredParams[i]);
                        
                        if (!areTypesCompatible(declaredType, paramTypes[i])) {
                            reportError("Function " + currentClassName + "::" + funcName + 
                                       " parameter " + std::to_string(i+1) + " type mismatch. Expected " + 
                                       formatTypeInfo(declaredType) + ", got " + formatTypeInfo(paramTypes[i]), node);
                        }
                    }
                }
                
                // Set return type
                currentExprType.type = funcSymbol->getType();
                currentExprType.dimensions.clear();
                currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
                return;
            }
        }
    }
    
    // Look for free function
    if (!funcSymbol || funcSymbol->getKind() != SymbolKind::FUNCTION) {
        reportError("Use of undeclared free function: " + funcName, node);
        currentExprType.type = "error";
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
        return;
    }
    
    // Check parameter count
    const auto& declaredParams = funcSymbol->getParams();
    if (declaredParams.size() != paramTypes.size()) {
        reportError("Function " + funcName + " called with wrong number of arguments. Expected " + 
                   std::to_string(declaredParams.size()) + ", got " + 
                   std::to_string(paramTypes.size()), node);
    } else {
        // Check each parameter type
        for (size_t i = 0; i < declaredParams.size(); ++i) {
            TypeInfo declaredType = parseTypeString(declaredParams[i]);
            
            if (!areTypesCompatible(declaredType, paramTypes[i])) {
                reportError("Function " + funcName + " parameter " + std::to_string(i+1) + 
                           " type mismatch. Expected " + formatTypeInfo(declaredType) + 
                           ", got " + formatTypeInfo(paramTypes[i]), node);
            }
        }
    }
    
    // Set return type
    currentExprType.type = funcSymbol->getType();
    currentExprType.dimensions.clear();
    currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
}

// Add this helper method:
std::shared_ptr<Symbol> SemanticCheckingVisitor::lookupFunctionCaseInsensitive(const std::string& name) {
    // First try exact match
    auto symbol = globalTable->lookupSymbol(name);
    if (symbol && symbol->getKind() == SymbolKind::FUNCTION) {
        return symbol;
    }
    
    // If not found, try case-insensitive match
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    for (const auto& [symName, symbol] : globalTable->getSymbols()) {
        if (symbol->getKind() == SymbolKind::FUNCTION) {
            std::string lowerSymName = symName;
            std::transform(lowerSymName.begin(), lowerSymName.end(), lowerSymName.begin(), ::tolower);
            
            if (lowerName == lowerSymName) {
                return symbol;
            }
        }
    }
    
    return nullptr;
}

// Identifier visitor - for checking identifiers
void SemanticCheckingVisitor::visitIdentifier(ASTNode* node) {
    std::string idName = node->getNodeValue();
    
    // First look for identifier in current function scope
    auto symbol = currentTable->lookupSymbol(idName);
    
    // If not found in function scope but we're in a class implementation,
    // check for class members (implicit self access)
    if (!symbol && !currentClassName.empty()) {
        auto classTable = globalTable->getNestedTable(currentClassName);
        if (classTable) {
            // Look for the member in this class
            symbol = classTable->lookupSymbol(idName);
            
            // If not found, check parent classes for inherited members
            if (!symbol) {
                auto classSymbol = globalTable->lookupSymbol(currentClassName);
                if (classSymbol && classSymbol->getKind() == SymbolKind::CLASS) {
                    const auto& inheritedClasses = classSymbol->getInheritedClasses();
                    for (const auto& parentClass : inheritedClasses) {
                        auto parentTable = globalTable->getNestedTable(parentClass);
                        if (parentTable) {
                            symbol = parentTable->lookupSymbol(idName);
                            if (symbol) break;
                        }
                    }
                }
            }
            
            if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
                // Found as class member - implicitly use "self"
                currentExprType.type = symbol->getType();
                currentExprType.dimensions = symbol->getArrayDimensions();
                currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
                return;
            }
        }
    }
    
    if (!symbol) {
        reportError("Use of undeclared variable: " + idName, node);
        currentExprType.type = "error";
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
        return;
    }
    
    // Set type information
    currentExprType.type = symbol->getType();
    currentExprType.dimensions = symbol->getArrayDimensions();
    currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
}

// Array access visitor - for checking array access
void SemanticCheckingVisitor::visitArrayAccess(ASTNode* node) {
    // Get array variable (left child) - either an identifier or dot_access
    ASTNode* arrayNode = node->getLeftMostChild();
    if (!arrayNode) {
        reportError("Missing array variable in array access", node);
        currentExprType.type = "error";
        return;
    }
    
    // Process the array variable to get its type
    arrayNode->accept(this);
    TypeInfo arrayType = currentExprType;
    
    // Check if this is actually an array
    if (arrayType.dimensions.empty()) {
        reportError("Array index used on non-array type: " + arrayType.type, node);
        currentExprType.type = "error";
        return;
    }
    
    // Process the index list (right child)
    ASTNode* indexListNode = arrayNode->getRightSibling();
    if (indexListNode) {
        // Process index list - this will check individual indices
        indexListNode->accept(this);
        
        // After processing all indices, update the result type
        // Remove the used dimensions based on how many indices were processed
        currentExprType = arrayType;
        
        // Count the indices
        int indexCount = 0;
        ASTNode* indexNode = indexListNode->getLeftMostChild();
        while (indexNode) {
            indexCount++;
            indexNode = indexNode->getRightSibling();
        }
        
        // Remove dimensions based on indices used
        if (indexCount <= currentExprType.dimensions.size()) {
            currentExprType.dimensions.erase(
                currentExprType.dimensions.begin(), 
                currentExprType.dimensions.begin() + indexCount
            );
        } else {
            // This is an error case, but it was already reported in visitIndexList
            currentExprType.dimensions.clear();
        }
    }
}

// Dot operator visitor - for checking member access
void SemanticCheckingVisitor::visitDotIdentifier(ASTNode* node) {
    // Check if the node itself contains the member name (direct member access in class context)
    std::string memberName = node->getNodeValue();
    if (!memberName.empty() && !currentClassName.empty()) {
        // This is a direct member access within a class method (implicit self)
        auto classTable = globalTable->getNestedTable(currentClassName);
        if (classTable) {
            auto memberSymbol = classTable->lookupSymbol(memberName);
            
            // If not found, check parent classes for inherited members
            if (!memberSymbol) {
                auto classSymbol = globalTable->lookupSymbol(currentClassName);
                if (classSymbol && classSymbol->getKind() == SymbolKind::CLASS) {
                    const auto& inheritedClasses = classSymbol->getInheritedClasses();
                    for (const auto& parentClass : inheritedClasses) {
                        auto parentTable = globalTable->getNestedTable(parentClass);
                        if (parentTable) {
                            memberSymbol = parentTable->lookupSymbol(memberName);
                            if (memberSymbol) break;
                        }
                    }
                }
            }
            
            if (memberSymbol && memberSymbol->getKind() == SymbolKind::VARIABLE) {
                // Found as class member - implicitly use "self"
                currentExprType.type = memberSymbol->getType();
                currentExprType.dimensions = memberSymbol->getArrayDimensions();
                currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
                return;
            }
        }
    }
    
    // Process object (if not direct member access)
    ASTNode* objNode = node->getLeftMostChild();
    if (!objNode) {
        // If we reached here, we couldn't handle it as a direct member
        // and there's no left child, so it's an error
        reportError("Missing object in dot expression", node);
        currentExprType.type = "error";
        return;
    }
    
    // Regular dot notation processing continues...
    objNode->accept(this);
    TypeInfo objType = currentExprType;
    
    // Check if this is a class type
    if (!objType.isClassType) {
        reportError("Dot operator used on non-class type: " + objType.type, node);
        currentExprType.type = "error";
        return;
    }
    
    // Process member (right side of the dot)
    ASTNode* memberNode = objNode->getRightSibling();
    if (!memberNode) {
        reportError("Missing member in dot expression", node);
        currentExprType.type = "error";
        return;
    }
    
    // Handle different types of member access
    if (memberNode->getNodeEnum() == NodeType::FUNCTION_CALL) {
        // Method call: object.method()
        handleMethodCall(objType, memberNode);
    } 
    else if (memberNode->getNodeEnum() == NodeType::DOT_IDENTIFIER) {
        // Nested property access: object.member.subMember
        memberNode->accept(this);
        // Type info is already set by the recursive accept call
    }
    else {
        // Simple member access: object.member
        std::string memberName = memberNode->getNodeValue();
        
        // Look up class
        auto classTable = globalTable->getNestedTable(objType.type);
        if (!classTable) {
            reportError("Class not found: " + objType.type, node);
            currentExprType.type = "error";
            return;
        }
        
        // Look up member
        auto memberSymbol = classTable->lookupSymbol(memberName);
        if (!memberSymbol) {
            reportError("Undeclared member: " + memberName + " in class " + objType.type, node);
            currentExprType.type = "error";
            return;
        }
        
        // Check visibility - only allow access to public members from outside
        if (memberSymbol->getVisibility() == Visibility::PRIVATE && currentClassName != objType.type) {
            reportError("Cannot access private member: " + memberName + " in class " + objType.type, node);
            currentExprType.type = "error";
            return;
        }
        
        // Set type information
        currentExprType.type = memberSymbol->getType();
        currentExprType.dimensions = memberSymbol->getArrayDimensions();
        currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
    }
}

void SemanticCheckingVisitor::visitDotAccess(ASTNode* node) {
    // Get the left part (object) and right part (member/method)
    ASTNode* objNode = node->getLeftMostChild();
    ASTNode* memberOrMethodNode = objNode ? objNode->getRightSibling() : nullptr;
    
    if (!objNode || !memberOrMethodNode) {
        reportError("Invalid dot access expression", node);
        currentExprType.type = "error";
        return;
    }
    
    // Process object to get its type
    objNode->accept(this);
    TypeInfo objType = currentExprType;
    
    // Check if this is a class type
    if (!objType.isClassType) {
        reportError("Dot operator used on non-class type: " + objType.type, node);
        currentExprType.type = "error";
        return;
    }
    
    // Different handling based on member node type
    if (memberOrMethodNode->getNodeEnum() == NodeType::FUNCTION_CALL) {
        // Method call: object.method() - directly use our method handling logic
        handleMethodCall(objType, memberOrMethodNode);
    } 
    else if (memberOrMethodNode->getNodeEnum() == NodeType::DOT_IDENTIFIER) {
        // Property access: object.member or object.member.submember
        memberOrMethodNode->accept(this);
        // Type is already set by the accept call
    }
    else if (memberOrMethodNode->getNodeEnum() == NodeType::ARRAY_ACCESS) {
        // Handle array access on a class member: object.member[index]
        
        // Get the array base (should be the left child of the ARRAY_ACCESS node)
        ASTNode* arrayBaseNode = memberOrMethodNode->getLeftMostChild();
        if (!arrayBaseNode) {
            reportError("Invalid array access in member expression", node);
            currentExprType.type = "error";
            return;
        }
        
        // Check if the array base is a member of the object's class
        if (arrayBaseNode->getNodeEnum() == NodeType::DOT_IDENTIFIER) {
            // Get the member name
            std::string memberName = arrayBaseNode->getNodeValue();
            
            // Look up class
            auto classTable = globalTable->getNestedTable(objType.type);
            if (!classTable) {
                reportError("Class not found: " + objType.type, node);
                currentExprType.type = "error";
                return;
            }
            
            // Look up member
            auto memberSymbol = classTable->lookupSymbol(memberName);
            if (!memberSymbol) {
                reportError("Undeclared member: " + memberName + " in class " + objType.type, node);
                currentExprType.type = "error";
                return;
            }
            
            // Check visibility - only allow access to public members from outside
            if (memberSymbol->getVisibility() == Visibility::PRIVATE && currentClassName != objType.type) {
                reportError("Cannot access private member: " + memberName + " in class " + objType.type, node);
                currentExprType.type = "error";
                return;
            }
            
            // Set temporary type information for the array base
            TypeInfo memberType;
            memberType.type = memberSymbol->getType();
            memberType.dimensions = memberSymbol->getArrayDimensions();
            memberType.isClassType = globalTable->lookupSymbol(memberType.type) != nullptr;
            
            // Check if member is an array
            if (memberType.dimensions.empty()) {
                reportError("Array index used on non-array member: " + memberName, node);
                currentExprType.type = "error";
                return;
            }
            
            // Now process the entire array access with this context
            memberOrMethodNode->accept(this);
            // currentExprType is now set by the visitArrayAccess method
        }
        else {
            // For other bases of the array access, just process normally
            memberOrMethodNode->accept(this);
        }
    }
    else if (memberOrMethodNode->getNodeEnum() == NodeType::IDENTIFIER) {
        // Simple member access: object.member
        std::string memberName = memberOrMethodNode->getNodeValue();
        
        // Look up class
        auto classTable = globalTable->getNestedTable(objType.type);
        if (!classTable) {
            reportError("Class not found: " + objType.type, node);
            currentExprType.type = "error";
            return;
        }
        
        // Look up member
        auto memberSymbol = classTable->lookupSymbol(memberName);
        if (!memberSymbol) {
            reportError("Undeclared member: " + memberName + " in class " + objType.type, node);
            currentExprType.type = "error";
            return;
        }
        
        // Check visibility - only allow access to public members from outside
        if (memberSymbol->getVisibility() == Visibility::PRIVATE && currentClassName != objType.type) {
            reportError("Cannot access private member: " + memberName + " in class " + objType.type, node);
            currentExprType.type = "error";
            return;
        }
        
        // Set type information
        currentExprType.type = memberSymbol->getType();
        currentExprType.dimensions = memberSymbol->getArrayDimensions();
        currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
    }
    else {
        reportError("Invalid member expression in dot access", node);
        currentExprType.type = "error";
    }
}

// Add this helper method:
void SemanticCheckingVisitor::handleMethodCall(const TypeInfo& objType, ASTNode* methodCallNode) {
    // Get method name
    ASTNode* methodIdNode = methodCallNode->getLeftMostChild();
    if (!methodIdNode) {
        reportError("Missing method name in method call", methodCallNode);
        currentExprType.type = "error";
        return;
    }
    
    std::string methodName = methodIdNode->getNodeValue();
    
    // Look up class
    auto classTable = globalTable->getNestedTable(objType.type);
    if (!classTable) {
        reportError("Class not found: " + objType.type, methodCallNode);
        currentExprType.type = "error";
        return;
    }
    
    // Look up method
    auto methodSymbol = classTable->lookupSymbol(methodName);
    if (!methodSymbol || methodSymbol->getKind() != SymbolKind::FUNCTION) {
        reportError("Undeclared method: " + methodName + " in class " + objType.type, methodCallNode);
        currentExprType.type = "error";
        return;
    }
    
    // Check visibility
    if (methodSymbol->getVisibility() == Visibility::PRIVATE && currentClassName != objType.type) {
        reportError("Cannot access private method: " + methodName + " in class " + objType.type, methodCallNode);
        currentExprType.type = "error";
        return;
    }
    
    // Process arguments
    std::vector<TypeInfo> argTypes;
    ASTNode* argsNode = methodIdNode->getRightSibling();
    if (argsNode) {
        ASTNode* arg = argsNode->getLeftMostChild();
        while (arg) {
            arg->accept(this);
            argTypes.push_back(currentExprType);
            arg = arg->getRightSibling();
        }
    }
    
    // Check parameter count
    const auto& paramTypes = methodSymbol->getParams();
    if (paramTypes.size() != argTypes.size()) {
        reportError("Method " + methodName + " called with wrong number of arguments. Expected " + 
                   std::to_string(paramTypes.size()) + ", got " + 
                   std::to_string(argTypes.size()), methodCallNode);
    }
    else {
        // Check parameter types
        for (size_t i = 0; i < paramTypes.size(); i++) {
            TypeInfo paramType = parseTypeString(paramTypes[i]);
            if (!areTypesCompatible(paramType, argTypes[i])) {
                reportError("Method " + methodName + " parameter " + std::to_string(i+1) + 
                           " type mismatch. Expected " + formatTypeInfo(paramType) + 
                           ", got " + formatTypeInfo(argTypes[i]), methodCallNode);
            }
        }
    }
    
    // Set return type
    currentExprType.type = methodSymbol->getType();
    currentExprType.dimensions.clear();
    currentExprType.isClassType = globalTable->lookupSymbol(currentExprType.type) != nullptr;
}

// Return statement visitor - for checking return types
void SemanticCheckingVisitor::visitReturnStatement(ASTNode* node) {
    // Process return expression
    ASTNode* exprNode = node->getLeftMostChild();
    if (exprNode) {
        exprNode->accept(this);
        TypeInfo returnType = currentExprType;
        
        // Check if we have an expected return type
        if (expectedReturnType.empty()) {
            reportError("Return statement outside function body", node);
            return;
        }
        
        // Check type compatibility
        TypeInfo expectedType;
        expectedType.type = expectedReturnType.back();
        
        if (!areTypesCompatible(expectedType, returnType)) {
            reportError("Return type mismatch. Expected " + expectedType.type + ", got " + returnType.type, node);
        }
    } else {
        // Void return
        if (!expectedReturnType.empty() && expectedReturnType.back() != "void") {
            reportError("Return type mismatch. Expected " + expectedReturnType.back() + ", got void", node);
        }
    }
}

// Function visitor - for handling function implementation
void SemanticCheckingVisitor::visitFunction(ASTNode* node) {
    // Check if we're at global scope or in a class
    bool isGlobalScope = (currentTable == globalTable);
    if (isGlobalScope) {
        currentClassName = "";
    }
    
    // Process function signature
    ASTNode* signatureNode = node->getLeftMostChild();
    if (signatureNode) {
        signatureNode->accept(this);
        
        // Set current return type expectation
        expectedReturnType.push_back(currentType);
        
        // Create properly formatted function table name
        std::string funcTableName;
        
        if (currentClassName.empty()) {
            // Global function
            funcTableName = currentFunctionName;
        } else {
            // Class member function - use qualified name
            funcTableName = currentFunctionName;
            
            // For overloaded functions, we need to include parameter types in the name
            auto functionSymbol = currentTable->lookupSymbol(currentFunctionName);
            if (functionSymbol && functionSymbol->getKind() == SymbolKind::FUNCTION) {
                const auto& params = functionSymbol->getParams();
                if (!params.empty()) {
                    // Append parameter types to distinguish overloaded functions
                    funcTableName += "_";
                    for (size_t i = 0; i < params.size(); i++) {
                        if (i > 0) funcTableName += "_";
                        funcTableName += params[i];
                    }
                }
            }
        }
        
        // Look up function table using the proper name
        auto functionTable = currentTable->getNestedTable(funcTableName);
        
        if (functionTable) {
            // Save current table and switch to function table
            auto savedTable = currentTable;
            currentTable = functionTable;
            
            // Process function body
            ASTNode* bodyNode = signatureNode->getRightSibling();
            if (bodyNode) {
                bodyNode->accept(this);
            }
            
            // Restore table
            currentTable = savedTable;
        }
        
        // Pop return type expectation
        if (!expectedReturnType.empty()) {
            expectedReturnType.pop_back();
        }
    }
}

// Helper methods for type checking

// Check if two types are compatible
bool SemanticCheckingVisitor::areTypesCompatible(const TypeInfo& type1, const TypeInfo& type2) {
    // Basic type compatibility
    if (type1.type != type2.type) {
        // Special case: numeric types can be compatible in some contexts
        // if (isNumericType(type1.type) && isNumericType(type2.type)) {
        //     return true;
        // }
        return false;
    }
    
    // Check array dimensions
    if (type1.dimensions.size() != type2.dimensions.size()) {
        return false;
    }
    
    // For each dimension, check compatibility
    for (size_t i = 0; i < type1.dimensions.size(); ++i) {
        // Dynamic arrays (-1) are compatible with any size
        if (type1.dimensions[i] != -1 && type2.dimensions[i] != -1 && 
            type1.dimensions[i] != type2.dimensions[i]) {
            return false;
        }
    }
    
    return true;
}

// Check if a type is numeric
bool SemanticCheckingVisitor::isNumericType(const std::string& type) {
    return type == "int" || type == "float";
}

// Parse a type string into TypeInfo
TypeInfo SemanticCheckingVisitor::parseTypeString(const std::string& typeStr) {
    TypeInfo result;
    
    // Extract base type and array dimensions
    std::regex arrayPattern("([^\\[]+)((?:\\[\\d*\\])*)");
    std::smatch matches;
    
    if (std::regex_match(typeStr, matches, arrayPattern)) {
        result.type = matches[1].str();
        
        // Extract array dimensions
        std::string dimensions = matches[2].str();
        std::regex dimPattern("\\[(\\d*)\\]");
        std::sregex_iterator it(dimensions.begin(), dimensions.end(), dimPattern);
        std::sregex_iterator end;
        
        while (it != end) {
            std::smatch dimMatch = *it;
            if (dimMatch[1].str().empty()) {
                // Dynamic array
                result.dimensions.push_back(-1);
            } else {
                // Fixed size array
                result.dimensions.push_back(std::stoi(dimMatch[1].str()));
            }
            ++it;
        }
    } else {
        result.type = typeStr;
    }
    
    // Check if it's a class type
    result.isClassType = globalTable->lookupSymbol(result.type) != nullptr;
    
    return result;
}

// Format TypeInfo for display
std::string SemanticCheckingVisitor::formatTypeInfo(const TypeInfo& typeInfo) {
    std::string result = typeInfo.type;
    
    for (int dim : typeInfo.dimensions) {
        if (dim == -1) {
            result += "[]";
        } else {
            result += "[" + std::to_string(dim) + "]";
        }
    }
    
    return result;
}

// Empty implementations for required visitor methods
void SemanticCheckingVisitor::visitEmpty(ASTNode* node) {}
void SemanticCheckingVisitor::visitClassList(ASTNode* node) {
    // Visit all children
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

// Get variable type information
TypeInfo SemanticCheckingVisitor::getVariableType(const std::string& name) {
    TypeInfo result;
    
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol) {
        result.type = symbol->getType();
        result.dimensions = symbol->getArrayDimensions();
        result.isClassType = globalTable->lookupSymbol(result.type) != nullptr;
    } else {
        result.type = "error";
    }
    
    return result;
}

// Visit functions for traversal and semantic checking

void SemanticCheckingVisitor::visitFunctionList(ASTNode* node) {
    // Reset class context when visiting global functions
    currentClassName = "";
    
    // Visit all functions in the list
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitImplementationList(ASTNode* node) {
    // Visit all implementation nodes
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
    
    // Reset class context
    currentClassName = "";
}

void SemanticCheckingVisitor::visitClass(ASTNode* node) {
    // Get class ID
    ASTNode* classIdNode = node->getLeftMostChild();
    if (!classIdNode) {
        reportError("Class declaration missing identifier.", node);
        return;
    }
    
    // Set current class
    currentClassName = classIdNode->getNodeValue();
    
    // Check if class exists in global table
    auto classSymbol = globalTable->lookupSymbol(currentClassName);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS) {
        reportError("Use of undeclared class: " + currentClassName, node);
        return;
    }
    
    // Switch to class table
    auto classTable = globalTable->getNestedTable(currentClassName);
    if (!classTable) {
        reportError("Symbol table missing for class: " + currentClassName, node);
        return;
    }
    
    auto savedTable = currentTable;
    currentTable = classTable;
    
    // Process inheritance list
    ASTNode* inheritanceNode = classIdNode->getRightSibling();
    if (inheritanceNode && inheritanceNode->getNodeEnum() == NodeType::INHERITANCE_LIST) {
        inheritanceNode->accept(this);
        
        // Process member list (third child)
        ASTNode* memberListNode = inheritanceNode->getRightSibling();
        if (memberListNode) {
            memberListNode->accept(this);
        }
    } else {
        // No inheritance list, process member list (second child)
        if (inheritanceNode) {
            inheritanceNode->accept(this);
        }
    }
    
    // Restore table
    currentTable = savedTable;
    currentClassName = "";
}

void SemanticCheckingVisitor::visitImplementation(ASTNode* node) {
    // Get implementation ID (class name)
    ASTNode* classIdNode = node->getLeftMostChild();
    if (!classIdNode) {
        reportError("Implementation missing class ID.", node);
        return;
    }
    
    std::string className = classIdNode->getNodeValue();
    
    // Check if the class exists in the global table
    auto classSymbol = globalTable->lookupSymbol(className);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS) {
        reportError("Implementation of undeclared class: " + className, node);
        return;
    }
    
    // Set current class name
    currentClassName = className;
    
    // Get class table
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable) {
        reportError("Symbol table missing for class: " + className, node);
        return;
    }
    
    // Save and switch to class table
    auto savedTable = currentTable;
    currentTable = classTable;
    
    // Process function list (second child)
    ASTNode* funcListNode = classIdNode->getRightSibling();
    if (funcListNode) {
        // Collect implemented function names for this class
        ASTNode* funcNode = funcListNode->getLeftMostChild();
        while (funcNode) {
            funcNode->accept(this);
            ASTNode* signatureNode = funcNode->getLeftMostChild();
            if (signatureNode) {
                ASTNode* funcIdNode = signatureNode->getLeftMostChild();
                if (funcIdNode) {
                    std::string funcName = funcIdNode->getNodeValue();
                    implementedFunctions[className].insert(funcName);
                }
            }
            funcNode = funcNode->getRightSibling();
        }
        
        funcListNode->accept(this);
    }
    
    // Restore table and class name
    currentTable = savedTable;
    currentClassName = "";
}

void SemanticCheckingVisitor::visitClassId(ASTNode* node) {
    // Store class name
    currentClassName = node->getNodeValue();
}

void SemanticCheckingVisitor::visitInheritanceList(ASTNode* node) {
    // Visit all inheritance IDs
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitInheritanceId(ASTNode* node) {
    std::string parentClassName = node->getNodeValue();
    
    // Check if parent class exists
    auto parentSymbol = globalTable->lookupSymbol(parentClassName);
    if (!parentSymbol || parentSymbol->getKind() != SymbolKind::CLASS) {
        reportError("Inheritance from undeclared class: " + parentClassName, node);
    }
}

void SemanticCheckingVisitor::visitVisibility(ASTNode* node) {
    // Only store visibility, no semantics to check
}

void SemanticCheckingVisitor::visitMemberList(ASTNode* node) {
    // Visit all members
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitMember(ASTNode* node) {
    // Process visibility
    ASTNode* visibilityNode = node->getLeftMostChild();
    if (visibilityNode) {
        visibilityNode->accept(this);
        
        // Process variable or function declaration
        ASTNode* memberNode = visibilityNode->getRightSibling();
        if (memberNode) {
            memberNode->accept(this);
        }
    }
}

void SemanticCheckingVisitor::visitVariable(ASTNode* node) {
    // Process variable ID
    ASTNode* varIdNode = node->getLeftMostChild();
    if (!varIdNode) {
        reportError("Variable declaration missing identifier.", node);
        return;
    }
    std::string varName = varIdNode->getNodeValue();

    // Process type
    ASTNode* typeNode = varIdNode->getRightSibling();
    if (!typeNode) {
        reportError("Variable declaration missing type.", node);
        return;
    }
    typeNode->accept(this); // Sets currentType

    // Process Dimension List (third child, optional)
    ASTNode* dimListNode = typeNode->getRightSibling();
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        // Visit dimensions if they exist
        if (dimListNode->getLeftMostChild()) {
             dimListNode->accept(this); // Collects dimensions temporarily if needed, though not strictly used here
        }
    }

    // Type checking for variable declarations primarily happens during symbol table creation
    // and later when the variable is used (via visitIdentifier).
    // We mainly ensure the structure is visited correctly here.
}

void SemanticCheckingVisitor::visitFunctionId(ASTNode* node) {
    // Store function name
    currentFunctionName = node->getNodeValue();
}

void SemanticCheckingVisitor::visitFunctionSignature(ASTNode* node) {
    // Process function ID
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (!funcIdNode) {
        reportError("Function signature missing identifier.", node);
        return;
    }
    
    funcIdNode->accept(this);
    
    // Process param list
    ASTNode* paramListNode = funcIdNode->getRightSibling();
    if (paramListNode) {
        paramListNode->accept(this);
    }
    
    // Process return type
    ASTNode* returnTypeNode = paramListNode ? paramListNode->getRightSibling() : nullptr;
    if (returnTypeNode) {
        returnTypeNode->accept(this);
    } else {
        // Default to void if no return type specified
        currentType = "void";
    }
}

void SemanticCheckingVisitor::visitFunctionBody(ASTNode* node) {
    // Process all statements in the function body
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitConstructorSignature(ASTNode* node) {
    // Similar to function signature but for constructors
    // Process class name as function name
    currentFunctionName = currentClassName;
    
    // Process param list
    ASTNode* paramListNode = node->getLeftMostChild();
    if (paramListNode) {
        paramListNode->accept(this);
    }
    
    // Constructor returns the class type
    currentType = currentClassName;
}

void SemanticCheckingVisitor::visitLocalVariable(ASTNode* node) {
    // // Process variable declaration
    // ASTNode* varNode = node->getLeftMostChild();
    // if (!varNode) return;
    
    // // Get variable ID
    // ASTNode* varIdNode = varNode->getLeftMostChild();
    // if (!varIdNode) return;
    
    // std::string varName = varIdNode->getNodeValue();
    
    // // Get type node
    // ASTNode* typeNode = varIdNode->getRightSibling();
    // if (!typeNode) return;
    
    // // Check for DUPLICATES BEFORE processing the declaration
    // auto localSymbol = currentTable->lookupSymbol(varName, true); // true = local scope only
    
    // if (localSymbol && localSymbol->getKind() == SymbolKind::VARIABLE) {
    //     reportError("Multiple declared identifier '" + varName + "' in function", node);
    // }
    
    // // Process the declaration normally (which will add it to the symbol table)
    // varNode->accept(this);
}

void SemanticCheckingVisitor::visitBlock(ASTNode* node) {
    // Process all statements in the block
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitIfStatement(ASTNode* node) {
    // Process condition
    ASTNode* condNode = node->getLeftMostChild();
    if (condNode) {
        condNode->accept(this);
        
        // Condition should be boolean
        if (currentExprType.type != "bool" && currentExprType.type != "int") {
            reportError("If condition must be boolean or numeric, got: " + currentExprType.type, node);
        }
        
        // Process then block
        ASTNode* thenNode = condNode->getRightSibling();
        if (thenNode) {
            thenNode->accept(this);
            
            // Process else block if present
            ASTNode* elseNode = thenNode->getRightSibling();
            if (elseNode) {
                elseNode->accept(this);
            }
        }
    }
}

void SemanticCheckingVisitor::visitWhileStatement(ASTNode* node) {
    // Process condition
    ASTNode* condNode = node->getLeftMostChild();
    if (condNode) {
        condNode->accept(this);
        
        // Condition should be boolean
        if (currentExprType.type != "bool" && currentExprType.type != "int") {
            reportError("While condition must be boolean or numeric, got: " + currentExprType.type, node);
        }
        
        // Process loop body
        ASTNode* bodyNode = condNode->getRightSibling();
        if (bodyNode) {
            bodyNode->accept(this);
        }
    }
}

void SemanticCheckingVisitor::visitRelationalExpr(ASTNode* node) {
    // Process left expression
    ASTNode* leftNode = node->getLeftMostChild();
    if (leftNode) {
        leftNode->accept(this);
        TypeInfo leftType = currentExprType;
        
        // Process relational operator
        ASTNode* opNode = leftNode->getRightSibling();
        
        // Process right expression
        ASTNode* rightNode = opNode ? opNode->getRightSibling() : nullptr;
        if (rightNode) {
            rightNode->accept(this);
            TypeInfo rightType = currentExprType;
            
            // Check type compatibility for relational operation
            if (!areTypesCompatible(leftType, rightType)) {
                reportError("Type mismatch in relational expression. Left side is " + 
                           formatTypeInfo(leftType) + " but right side is " + formatTypeInfo(rightType), node);
            }
            
            // Result is boolean
            currentExprType.type = "bool";
            currentExprType.dimensions.clear();
            currentExprType.isClassType = false;
        }
    }
}

void SemanticCheckingVisitor::visitFunctionDeclaration(ASTNode* node) {
    // Process function signature
    ASTNode* signatureNode = node->getLeftMostChild();
    if (!signatureNode) return;
    
    // Get function name
    ASTNode* funcIdNode = signatureNode->getLeftMostChild();
    if (!funcIdNode) return;
    
    std::string funcName = funcIdNode->getNodeValue();
    
    // Store in declared functions for current class
    if (!currentClassName.empty()) {
        declaredFunctions[currentClassName].insert(funcName);
    }
    
    // Continue with normal processing
    signatureNode->accept(this);
}

void SemanticCheckingVisitor::visitAttribute(ASTNode* node) {
    // Just visit the child
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SemanticCheckingVisitor::visitSingleStatement(ASTNode* node) {
    // Visit the contained statement
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SemanticCheckingVisitor::visitExpressionStatement(ASTNode* node) {
    // Visit the expression
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SemanticCheckingVisitor::visitReadStatement(ASTNode* node) {
    // Process variable to read into
    ASTNode* varNode = node->getLeftMostChild();
    if (varNode) {
        varNode->accept(this);
        
        // Check if variable is readable type (int, float)
        if (!isNumericType(currentExprType.type) && currentExprType.type != "string") {
            reportError("Cannot read into variable of type: " + currentExprType.type, node);
        }
    }
}

void SemanticCheckingVisitor::visitWriteStatement(ASTNode* node) {
    // Process expression to write
    ASTNode* exprNode = node->getLeftMostChild();
    if (exprNode) {
        exprNode->accept(this);
        
        // Check if expression is writable type (int, float, string)
        if (!isNumericType(currentExprType.type) && currentExprType.type != "string") {
            reportWarning("Writing non-standard type: " + currentExprType.type, node);
        }
    }
}

void SemanticCheckingVisitor::visitAssignOp(ASTNode* node) {
    // Nothing to check for assignment operator itself
}

void SemanticCheckingVisitor::visitRelOp(ASTNode* node) {
    // Nothing to check for relational operator itself
}

void SemanticCheckingVisitor::visitAddOp(ASTNode* node) {
    // Get left operand
    ASTNode* leftNode = node->getLeftMostChild();
    if (!leftNode) {
        currentExprType.type = "error";
        return;
    }
    
    // Process left operand
    leftNode->accept(this);
    TypeInfo leftType = currentExprType;
    
    // Get right operand
    ASTNode* rightNode = leftNode->getRightSibling();
    if (!rightNode) {
        currentExprType.type = "error";
        return;
    }
    
    // Process right operand
    rightNode->accept(this);
    TypeInfo rightType = currentExprType;
    
    // Check for numeric types on both sides of addition
    if (!isNumericType(leftType.type) || !isNumericType(rightType.type)) {
        reportError("Type error in addition: requires numeric types, got " + 
                  formatTypeInfo(leftType) + " and " + formatTypeInfo(rightType), node);
        currentExprType.type = "error"; // Set to error type
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
        return; // Stop processing after error
    }
    
    // Check if the types are exactly the same
    if (leftType.type != rightType.type) {
        reportError("Type error in addition: operands must have identical types, got " + 
                  formatTypeInfo(leftType) + " and " + formatTypeInfo(rightType), node);
        currentExprType.type = "error"; // Set to error type
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
        return; // Stop processing after error
    }
    
    // Types are compatible, set result type (same as input types)
    currentExprType.type = leftType.type;
    currentExprType.dimensions.clear();
    currentExprType.isClassType = false;
}

void SemanticCheckingVisitor::visitMultOp(ASTNode* node) {
    // Get left operand
    ASTNode* leftNode = node->getLeftMostChild();
    if (!leftNode) {
        currentExprType.type = "error";
        return;
    }
    
    // Process left operand
    leftNode->accept(this);
    TypeInfo leftType = currentExprType;
    
    // Get right operand
    ASTNode* rightNode = leftNode->getRightSibling();
    if (!rightNode) {
        currentExprType.type = "error";
        return;
    }
    
    // Process right operand
    rightNode->accept(this);
    TypeInfo rightType = currentExprType;
    
    // Check for numeric types on both sides of multiplication
    if (!isNumericType(leftType.type) || !isNumericType(rightType.type)) {
        reportError("Type error in multiplication: requires numeric types, got " + 
                  formatTypeInfo(leftType) + " and " + formatTypeInfo(rightType), node);
        currentExprType.type = "error"; // Set to error type
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
        return; // Stop processing after error
    }
    
    // Check if the types are exactly the same
    if (leftType.type != rightType.type) {
        reportError("Type error in multiplication: operands must have identical types, got " + 
                  formatTypeInfo(leftType) + " and " + formatTypeInfo(rightType), node);
        currentExprType.type = "error"; // Set to error type
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
        return; // Stop processing after error
    }
    
    // Types are compatible, set result type (same as input types)
    currentExprType.type = leftType.type;
    currentExprType.dimensions.clear();
    currentExprType.isClassType = false;
}

void SemanticCheckingVisitor::visitSelfIdentifier(ASTNode* node) {
    // Check if we're in a class context
    if (currentClassName.empty()) {
        reportError("'self' used outside of class context", node);
        currentExprType.type = "error";
        return;
    }
    
    // 'self' has the type of the current class
    currentExprType.type = currentClassName;
    currentExprType.dimensions.clear();
    currentExprType.isClassType = true;
}

void SemanticCheckingVisitor::visitType(ASTNode* node) {
    // Set current type
    currentType = node->getNodeValue();
    
    // Reset array dimensions
    currentArrayDimensions.clear();
}

void SemanticCheckingVisitor::visitArrayDimension(ASTNode* node) {
    // Store array dimension
    try {
        if (node->getNodeValue() == "dynamic") {
            currentArrayDimensions.push_back(-1);
        } else {
            int dim = std::stoi(node->getNodeValue());
            currentArrayDimensions.push_back(dim);
        }
    } catch (const std::exception&) {
        reportError("Invalid array dimension: " + node->getNodeValue(), node);
    }
}

void SemanticCheckingVisitor::visitParam(ASTNode* node) {
    // Process parameter id
    ASTNode* paramIdNode = node->getLeftMostChild();
    if (!paramIdNode) {
        reportError("Parameter missing identifier.", node);
        return;
    }

    // Process parameter type
    ASTNode* typeNode = paramIdNode->getRightSibling();
    if (!typeNode) {
        reportError("Parameter missing type.", node);
        return;
    }
    typeNode->accept(this); // Sets currentType

    // Process Dimension List (third child, optional)
    ASTNode* dimListNode = typeNode->getRightSibling();
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        // Visit dimensions if they exist
        if (dimListNode->getLeftMostChild()) {
             dimListNode->accept(this); // Collects dimensions temporarily
        }
    }

    // Type checking for parameters happens during function call checks (visitFunctionCall)
    // by comparing argument types with the parameter types stored in the function symbol.
}

void SemanticCheckingVisitor::visitFactor(ASTNode* node) {
    // Visit the factor's child
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
        // Type is inherited from child
    } else {
        // Default to error type if no child
        currentExprType.type = "error";
        currentExprType.dimensions.clear();
        currentExprType.isClassType = false;
    }
}

void SemanticCheckingVisitor::visitTerm(ASTNode* node) {
    // Process first factor
    ASTNode* factorNode = node->getLeftMostChild();
    if (!factorNode) {
        currentExprType.type = "error";
        return;
    }
    
    // Process the first factor
    factorNode->accept(this);
    TypeInfo leftType = currentExprType;
    
    // Check for additional factors that might involve multiplication
    ASTNode* currentNode = factorNode->getRightSibling();
    while (currentNode) {
        // Check if this is a multiplication operator
        if (currentNode->getNodeEnum() == NodeType::MULT_OP) {
            // Get the right operand
            ASTNode* rightOperand = currentNode->getRightSibling();
            if (rightOperand) {
                // Process right operand
                rightOperand->accept(this);
                TypeInfo rightType = currentExprType;
                
                // Check for numeric types on both sides of multiplication
                if (!isNumericType(leftType.type) || !isNumericType(rightType.type)) {
                    reportError("Type error in multiplication: requires numeric types, got " + 
                              formatTypeInfo(leftType) + " and " + formatTypeInfo(rightType), currentNode);
                }
                
                // Result type is float if either operand is float, otherwise int
                if (leftType.type == "float" || rightType.type == "float") {
                    currentExprType.type = "float";
                } else {
                    currentExprType.type = "int";
                }
                currentExprType.dimensions.clear();
                currentExprType.isClassType = false;
                
                // Update left type for next operation
                leftType = currentExprType;
                
                // Move to next node
                currentNode = rightOperand->getRightSibling();
            } else {
                // Missing right operand
                reportError("Missing right operand for multiplication", currentNode);
                break;
            }
        } else {
            // Not a multiplication operator, move to next node
            currentNode = currentNode->getRightSibling();
        }
    }
}

void SemanticCheckingVisitor::visitArithExpr(ASTNode* node) {
    // Process first term
    ASTNode* termNode = node->getLeftMostChild();
    if (!termNode) {
        currentExprType.type = "error";
        return;
    }
    
    // Process the first term
    termNode->accept(this);
    TypeInfo leftType = currentExprType;
    
    // Check for additional terms that might involve addition
    ASTNode* currentNode = termNode->getRightSibling();
    while (currentNode) {
        // Check if this is an addition operator
        if (currentNode->getNodeEnum() == NodeType::ADD_OP) {
            // Get the right operand
            ASTNode* rightOperand = currentNode->getRightSibling();
            if (rightOperand) {
                // Process right operand
                rightOperand->accept(this);
                TypeInfo rightType = currentExprType;
                
                // Check for numeric types on both sides of addition
                if (!isNumericType(leftType.type) || !isNumericType(rightType.type)) {
                    reportError("Type error in addition: requires numeric types, got " + 
                              formatTypeInfo(leftType) + " and " + formatTypeInfo(rightType), currentNode);
                }
                
                // Result type is float if either operand is float, otherwise int
                if (leftType.type == "float" || rightType.type == "float") {
                    currentExprType.type = "float";
                } else {
                    currentExprType.type = "int";
                }
                currentExprType.dimensions.clear();
                currentExprType.isClassType = false;
                
                // Update left type for next operation
                leftType = currentExprType;
                
                // Move to next node
                currentNode = rightOperand->getRightSibling();
            } else {
                // Missing right operand
                reportError("Missing right operand for addition", currentNode);
                break;
            }
        } else {
            // Not an addition operator, move to next node
            currentNode = currentNode->getRightSibling();
        }
    }
}
void SemanticCheckingVisitor::visitExpr(ASTNode* node) {
    // Visit the contained expression
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
        // Type is inherited from child
    }
}

void SemanticCheckingVisitor::visitImplementationId(ASTNode* node) {
    // Store implementation ID (class name)
    currentClassName = node->getNodeValue();
    
    // Check if class exists
    auto classSymbol = globalTable->lookupSymbol(currentClassName);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS) {
        reportError("Implementation of undeclared class: " + currentClassName, node);
    }
}

void SemanticCheckingVisitor::visitVariableId(ASTNode* node) {
    // Nothing to check for variable ID itself
}

void SemanticCheckingVisitor::visitParamList(ASTNode* node) {
    // Visit all parameters
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitParamId(ASTNode* node) {
    // Nothing to check for parameter ID itself
}

void SemanticCheckingVisitor::visitFloat(ASTNode* node) {
    // Set type for float literal
    currentExprType.type = "float";
    currentExprType.dimensions.clear();
    currentExprType.isClassType = false;
}

void SemanticCheckingVisitor::visitInt(ASTNode* node) {
    // Set type for integer literal
    currentExprType.type = "int";
    currentExprType.dimensions.clear();
    currentExprType.isClassType = false;
}

void SemanticCheckingVisitor::visitStatementsList(ASTNode* node) {
    // Visit all statements
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitImplementationFunctionList(ASTNode* node) {
    // Visit all implementation functions
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitCondition(ASTNode* node) {
    // Visit condition expression
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
        
        // Condition should be boolean or numeric
        if (currentExprType.type != "bool" && currentExprType.type != "int") {
            reportError("Condition must be boolean or numeric, got: " + currentExprType.type, node);
        }
    }
}

void SemanticCheckingVisitor::visitArrayType(ASTNode* node) {
    // Process array dimension
    ASTNode* dimNode = node->getLeftMostChild();
    if (dimNode) {
        dimNode->accept(this);
        
        // Process base type
        ASTNode* typeNode = dimNode->getRightSibling();
        if (typeNode) {
            typeNode->accept(this);
        }
    }
}

// The error reporting methods
void SemanticCheckingVisitor::reportError(const std::string& message, ASTNode* node) {
    SemanticError error;
    error.message = "SemCheck Error: " + message;
    error.line = node ? node->getLineNumber() : 0;
    error.isWarning = false;
    
    semanticErrors.push_back(error);
    std::cerr << "Error at line " << error.line << ": " << message << std::endl;
}

void SemanticCheckingVisitor::reportWarning(const std::string& message, ASTNode* node) {
    SemanticError error;
    error.message = "SemCheck Warning: " + message;
    error.line = node ? node->getLineNumber() : 0;
    error.isWarning = true;
    
    semanticErrors.push_back(error);
    std::cout << "Warning at line " << error.line << ": " << message << std::endl;
}

bool SemanticCheckingVisitor::hasErrors() const {
    for (const auto& error : semanticErrors) {
        if (!error.isWarning) {
            return true;
        }
    }
    return false;
}

// New method to output errors to a file
void SemanticCheckingVisitor::outputErrors(const std::string& originalFilename) {
    if (semanticErrors.empty()) {
        return;  // No errors to report
    }
    
    // Sort errors by line number and then column number for synchronized order
    std::sort(semanticErrors.begin(), semanticErrors.end(), 
              [](const SemanticError& a, const SemanticError& b) {    
                return a.line < b.line;
              });
    
    // Create output file
    std::string outputFilename = originalFilename + ".outsemanticerrors";
    std::ofstream outFile(outputFilename);
    
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputFilename << std::endl;
        return;
    }
    
    outFile << "=== Semantic Errors/Warnings for " << originalFilename << " ===" << std::endl;
    outFile << std::endl;
    
    for (const auto& error : semanticErrors) {
        outFile << (error.isWarning ? "Warning" : "Error") << " at line " << error.line 
                << ": " << error.message << std::endl;
    }
    
    outFile.close();
    std::cout << "Semantic errors/warnings written to: " << outputFilename << std::endl;
}

// Implementation of new methods

void SemanticCheckingVisitor::checkDuplicateClassDeclarations() {
    std::unordered_map<std::string, int> classCount;
    
    // Count occurrences of each class name
    for (const auto& [className, table] : globalTable->getNestedTables()) {
        auto classSymbol = globalTable->lookupSymbol(className);
        if (classSymbol && classSymbol->getKind() == SymbolKind::CLASS) {
            classCount[className]++;
        }
    }
    
    // Report duplicate class declarations
    for (const auto& [className, count] : classCount) {
        if (count > 1) {
            reportError("Multiple declared class: " + className, nullptr);
        }
    }
}

void SemanticCheckingVisitor::checkDuplicateFunctionDeclarations() {
    std::unordered_map<std::string, std::vector<std::shared_ptr<Symbol>>> functionSymbols;
    
    // Collect function symbols with same name
    for (const auto& [funcName, symbol] : globalTable->getSymbols()) {
        if (symbol->getKind() == SymbolKind::FUNCTION) {
            functionSymbols[funcName].push_back(symbol);
        }
    }
    
    // Check for functions with same name and signature
    for (const auto& [funcName, symbols] : functionSymbols) {
        for (size_t i = 0; i < symbols.size(); i++) {
            for (size_t j = i + 1; j < symbols.size(); j++) {
                const auto& params1 = symbols[i]->getParams();
                const auto& params2 = symbols[j]->getParams();
                
                // If same name, same param count, and same param types - it's a duplicate
                if (params1.size() == params2.size()) {
                    bool allSame = true;
                    for (size_t k = 0; k < params1.size(); k++) {
                        if (params1[k] != params2[k]) {
                            allSame = false;
                            break;
                        }
                    }
                    
                    if (allSame && symbols[i]->getType() == symbols[j]->getType()) {
                        reportError("e defined free function: " + funcName, nullptr);
                    }
                }
            }
        }
    }
}

void SemanticCheckingVisitor::checkDuplicateMemberDeclarations(const std::string& className) {
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable) return;
    
    std::unordered_map<std::string, std::vector<std::shared_ptr<Symbol>>> memberSymbols;
    
    // Collect member symbols with same name
    for (const auto& [memberName, symbol] : classTable->getSymbols()) {
        if (symbol->getKind() == SymbolKind::VARIABLE) {
            memberSymbols[memberName].push_back(symbol);
        }
    }
    
    // Report duplicate member declarations
    for (const auto& [memberName, symbols] : memberSymbols) {
        if (symbols.size() > 1) {
            reportError("Multiple declared identifier '" + memberName + "' in class " + className, nullptr);
        }
    }
}

void SemanticCheckingVisitor::checkShadowedInheritedMembers(const std::string& className) {
    auto classSymbol = globalTable->lookupSymbol(className);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS) return;
    
    const auto& inheritedClasses = classSymbol->getInheritedClasses();
    if (inheritedClasses.empty()) return;
    
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable) return;
    
    // Get parent class table
    std::string parentClass = inheritedClasses[0];
    auto parentTable = globalTable->getNestedTable(parentClass);
    if (!parentTable) return;
    
    // Check for shadowed members
    for (const auto& [memberName, memberSymbol] : classTable->getSymbols()) {
        if (memberSymbol->getKind() == SymbolKind::VARIABLE) {
            auto parentMember = parentTable->lookupSymbol(memberName);
            if (parentMember && parentMember->getKind() == SymbolKind::VARIABLE) {
                reportError("Data member '" + memberName + "' in class " + className + 
                          " shadows inherited member from class " + parentClass, nullptr);
            }
        }
    }
}

void SemanticCheckingVisitor::checkUndefinedMemberFunctions() {
    // For each class, check if all declared functions are implemented
    for (const auto& [className, declared] : declaredFunctions) {
        auto implemented = implementedFunctions[className];
        
        for (const auto& funcName : declared) {
            if (implemented.find(funcName) == implemented.end()) {
                reportError("Undefined member function: " + funcName + " in class " + className, nullptr);
            }
        }
    }
}

void SemanticCheckingVisitor::checkUndeclaredMemberFunctions() {
    // For each class, check if all implemented functions are declared
    for (const auto& [className, implemented] : implementedFunctions) {
        auto declared = declaredFunctions[className];
        
        for (const auto& funcName : implemented) {
            if (declared.find(funcName) == declared.end()) {
                reportError("Undeclared member function definition: " + funcName + " in class " + className, nullptr);
            }
        }
    }
}

void SemanticCheckingVisitor::importSymbolTableErrors(const SymbolTableVisitor& symbolTableVisitor) {
    // Import all errors from the symbol table visitor with line information
    for (const auto& errorInfo : symbolTableVisitor.getErrors()) {
        reportError(errorInfo.message, errorInfo.line);
    }
    
    // Import all warnings from the symbol table visitor with line information
    for (const auto& warningInfo : symbolTableVisitor.getWarnings()) {
        reportWarning(warningInfo.message, warningInfo.line);
    }
}

// Add a method to report errors with explicit line numbers
void SemanticCheckingVisitor::reportError(const std::string& message, int line) {
    SemanticError error;
    error.message = message;
    error.line = line;
    error.isWarning = false;
    semanticErrors.push_back(error);
    
    std::cerr << "Error" << (line > 0 ? " at line " + std::to_string(line) : "") 
              << ": " << message << std::endl;
}

// Add a method to report warnings with explicit line numbers
void SemanticCheckingVisitor::reportWarning(const std::string& message, int line) {
    SemanticError warning;
    warning.message = message;
    warning.line = line;
    warning.isWarning = true;
    semanticErrors.push_back(warning);
    
    std::cerr << "Warning" << (line > 0 ? " at line " + std::to_string(line) : "") 
              << ": " << message << std::endl;
}

void SemanticCheckingVisitor::visitDimList(ASTNode* node) {
    // Clear any existing array dimensions before processing
    currentArrayDimensions.clear();
    
    // Process each dimension node in the list
    ASTNode* dimNode = node->getLeftMostChild();
    while (dimNode) {
        // Process this dimension
        dimNode->accept(this);
        // Each dimension's visit will add to the currentArrayDimensions vector
        
        // Move to next dimension
        dimNode = dimNode->getRightSibling();
    }
}

void SemanticCheckingVisitor::visitIndexList(ASTNode* node) {
    // Count the number of index expressions in the list
    int indexCount = 0;
    ASTNode* indexNode = node->getLeftMostChild();

    // Process each index expression and count them
    while (indexNode) {
        // Process this index expression
        indexNode->accept(this);

        // *** ADD THIS CHECK BACK ***
        // Check that the index expression evaluates to an integer
        if (currentExprType.type != "int") {
            reportError("Array index must be of integer type, found: " + formatTypeInfo(currentExprType), indexNode);
        }
        // *** END ADDED CHECK ***

        indexCount++;
        indexNode = indexNode->getRightSibling();
    }

    // Get the array variable node (sibling of the index list in the parent ARRAY_ACCESS node)
    ASTNode* arrayNode = node->getLeftMostSibling();
    if (arrayNode) {
        // Temporarily store the type result from the index processing (likely 'int' or 'error')
        TypeInfo indexResultType = currentExprType;

        // Visit the array variable to get its type information
        arrayNode->accept(this);
        TypeInfo arrayVarType = currentExprType; // Now currentExprType holds the array's type info

        // Check if the number of indices matches the number of dimensions
        int dimensionsCount = arrayVarType.dimensions.size();

        if (indexCount != dimensionsCount) {
            reportError("Incorrect number of array indices. Expected " +
                       std::to_string(dimensionsCount) + ", got " +
                       std::to_string(indexCount), node);
        }

        // Restore currentExprType if needed, though visitArrayAccess will set it correctly afterwards.
        // currentExprType = indexResultType; // Usually not necessary as parent node dictates final type
    }
}