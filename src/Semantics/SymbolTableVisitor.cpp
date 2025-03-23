#include "SymbolTableVisitor.h"
#include <iostream>
#include <sstream>
#include <algorithm> 

// FunctionSignature implementation
bool FunctionSignature::operator==(const FunctionSignature& other) const {
    if (name != other.name || paramTypes.size() != other.paramTypes.size())
        return false;
    
    for (size_t i = 0; i < paramTypes.size(); i++) {
        if (paramTypes[i] != other.paramTypes[i])
            return false;
    }
    return true;
}

// Hash function implementation for FunctionSignature
namespace std {
    std::size_t hash<FunctionSignature>::operator()(const FunctionSignature& sig) const {
        std::size_t h = std::hash<std::string>{}(sig.name);
        for (const auto& param : sig.paramTypes) {
            h ^= std::hash<std::string>{}(param) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
}

// Symbol implementation
Symbol::Symbol(const std::string& name, const std::string& type, SymbolKind kind)
    : name(name), type(type), kind(kind) {}

// SymbolTable implementation
SymbolTable::SymbolTable(const std::string& scopeName, SymbolTable* parent)
    : scopeName(scopeName), parent(parent) {}
    
bool SymbolTable::addSymbol(std::shared_ptr<Symbol> symbol) {
    const std::string& name = symbol->getName();
    if (symbols.find(name) != symbols.end()) {
        return false; // Symbol already exists
    }
    
    symbols[name] = symbol;
    
    // If it's a function, add it to the function map too
    if (symbol->getKind() == SymbolKind::FUNCTION) {
        FunctionSignature sig;
        sig.name = name;
        sig.paramTypes = symbol->getParams();
        functions[sig] = symbol;
    }
    
    return true;
}

std::shared_ptr<Symbol> SymbolTable::lookupSymbol(const std::string& name, bool localOnly) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    
    if (localOnly || !parent) {
        return nullptr;
    }
    
    return parent->lookupSymbol(name);
}

std::shared_ptr<Symbol> SymbolTable::lookupFunction(const FunctionSignature& signature, bool localOnly) {
    auto it = functions.find(signature);
    if (it != functions.end()) {
        return it->second;
    }
    
    if (localOnly || !parent) {
        return nullptr;
    }
    
    return parent->lookupFunction(signature);
}

std::vector<std::shared_ptr<Symbol>> SymbolTable::lookupFunctions(const std::string& name, bool localOnly) {
    std::vector<std::shared_ptr<Symbol>> result;
    
    for (const auto& pair : functions) {
        if (pair.first.name == name) {
            result.push_back(pair.second);
        }
    }
    
    if (!localOnly && parent) {
        auto parentResults = parent->lookupFunctions(name);
        result.insert(result.end(), parentResults.begin(), parentResults.end());
    }
    
    return result;
}

void SymbolTable::addNestedTable(const std::string& name, std::shared_ptr<SymbolTable> table) {
    nestedTables[name] = table;
}

std::shared_ptr<SymbolTable> SymbolTable::getNestedTable(const std::string& name) {
    auto it = nestedTables.find(name);
    if (it != nestedTables.end()) {
        return it->second;
    }
    return nullptr;
}

// SymbolTableVisitor implementation
SymbolTableVisitor::SymbolTableVisitor() {
    // Initialize with empty tables and state
}

SymbolTableVisitor::~SymbolTableVisitor() {
    // Clean up if needed
}

void SymbolTableVisitor::reportError(const std::string& message) {
    errors.push_back("Error: " + message);
    std::cerr << "Error: " << message << std::endl;
}

void SymbolTableVisitor::reportWarning(const std::string& message) {
    warnings.push_back("Warning: " + message);
    std::cout << "Warning: " << message << std::endl;
}

void SymbolTableVisitor::visitProgram(ASTNode* node) {
    // Create global table
    globalTable = std::make_shared<SymbolTable>("global");
    currentTable = globalTable;
    
    // Visit all children of the program node
    ASTNode* currentChild = node->getLeftMostChild();
    while (currentChild) {
        currentChild->accept(this);
        currentChild = currentChild->getRightSibling();
    }
    
    // Check for function consistency after the whole program is processed
    checkFunctionConsistency();
}

void SymbolTableVisitor::visitClassList(ASTNode* node) {
    // Traverse all classes in the list
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitClass(ASTNode* node) {
    // Get class name from ClassId child
    ASTNode* classIdNode = node->getLeftMostChild();
    classIdNode->accept(this);
    
    // Create a new symbol for this class
    auto classSymbol = std::make_shared<Symbol>(currentClassName, currentClassName, SymbolKind::CLASS);
    
    // Check if class is already defined
    if (globalTable->lookupSymbol(currentClassName, true)) {
        reportError("Multiply declared class: " + currentClassName);
        return;
    }
    
    // Add class to global table
    globalTable->addSymbol(classSymbol);
    
    // Create a new table for this class
    auto classTable = std::make_shared<SymbolTable>(currentClassName, globalTable.get());
    globalTable->addNestedTable(currentClassName, classTable);
    
    // Save current table and switch to class table
    auto savedTable = currentTable;
    currentTable = classTable;
    
    // Process inheritance
    ASTNode* inheritanceNode = classIdNode->getRightSibling();
    if (inheritanceNode && inheritanceNode->getNodeEnum() == NodeType::INHERITANCE_LIST) {
        inheritanceNode->accept(this);
    }
    
    // Process members with default visibility
    currentVisibility = Visibility::PRIVATE;
    ASTNode* memberListNode = inheritanceNode ? inheritanceNode->getRightSibling() : classIdNode->getRightSibling();
    if (memberListNode) {
        memberListNode->accept(this);
    }
    
    // Check for shadowed members
    checkShadowedMembers(currentClassName);
    
    // Restore current table
    currentTable = savedTable;
}

void SymbolTableVisitor::visitClassId(ASTNode* node) {
    currentClassName = node->getNodeValue();
}

void SymbolTableVisitor::visitInheritanceList(ASTNode* node) {
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitInheritanceId(ASTNode* node) {
    std::string inheritedClass = node->getNodeValue();
    
    // Check if the inherited class exists
    auto inheritedClassSymbol = globalTable->lookupSymbol(inheritedClass);
    if (!inheritedClassSymbol || inheritedClassSymbol->getKind() != SymbolKind::CLASS) {
        reportError("Class " + currentClassName + " inherits from undefined class: " + inheritedClass);
        return;
    }
    
    // Add the inheritance relationship
    auto classSymbol = globalTable->lookupSymbol(currentClassName);
    if (classSymbol) {
        classSymbol->addInheritedClass(inheritedClass);
    }
}

void SymbolTableVisitor::visitVisibility(ASTNode* node) {
    std::string visibility = node->getNodeValue();
    if (visibility == "public") {
        currentVisibility = Visibility::PUBLIC;
    } else {
        currentVisibility = Visibility::PRIVATE;
    }
}

void SymbolTableVisitor::visitMemberList(ASTNode* node) {
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitFunction(ASTNode* node) {
    // Check if we're at global scope to ensure proper function naming
    bool isGlobalScope = (currentTable == globalTable);
    // If we're at global scope, make sure currentClassName is empty
    if (isGlobalScope) {
        currentClassName = "";
    }
    bool isImplementation = !currentClassName.empty() && currentTable->getScopeName() == currentClassName;
    
    // Process function signature (left child)
    ASTNode* signatureNode = node->getLeftMostChild();
    if (!signatureNode) {
        reportError("Function missing signature.");
        return;
    }
    
    // Clear param types and process signature
    currentParamTypes.clear();
    signatureNode->accept(this);
    
    // Create function signature for lookup
    FunctionSignature sig;
    sig.name = currentFunctionName;
    sig.paramTypes = currentParamTypes;
    
    if (isImplementation) {
        // This is an implementation of a member function
        // Look up the existing function declaration
        auto existingFunction = currentTable->lookupFunction(sig, true);
        if (!existingFunction) {
            reportError("Implementation of undeclared member function: " + 
                      currentClassName + "::" + currentFunctionName);
            return;
        }
        
        // Mark the function as defined
        existingFunction->setDefined(true);
        
        // Create or get the function's symbol table
        std::string funcTableName = currentClassName + "::" + currentFunctionName;
        auto functionTable = currentTable->getNestedTable(currentFunctionName);
        if (!functionTable) {
            // Create a new table if one doesn't exist yet
            functionTable = std::make_shared<SymbolTable>(funcTableName, currentTable.get());
            currentTable->addNestedTable(currentFunctionName, functionTable);
        }
        
        // Process function body with the function table as current
        auto savedTable = currentTable;
        currentTable = functionTable;
        
        // The function signature should have created the parameter list, but we need to
        // make sure they're added to the function table. Reprocess param list:
        ASTNode* paramListNode = signatureNode->getLeftMostChild()->getRightSibling();
        if (paramListNode && paramListNode->getNodeEnum() == NodeType::PARAM_LIST) {
            ASTNode* param = paramListNode->getLeftMostChild();
            while (param) {
                param->accept(this);
                param = param->getRightSibling();
            }
        }
        
        // Process function body (right child)
        ASTNode* bodyNode = signatureNode->getRightSibling();
        if (bodyNode) {
            bodyNode->accept(this);
        }
        
        // Restore current table
        currentTable = savedTable;
    } else {
        // This is a new function declaration/definition
        
        // Create function symbol
        auto functionSymbol = std::make_shared<Symbol>(currentFunctionName, currentType, SymbolKind::FUNCTION);
        functionSymbol->setParams(currentParamTypes);
        functionSymbol->setVisibility(currentVisibility);
        functionSymbol->setDeclared(true);
        functionSymbol->setDefined(true);  // Since this is a function definition, not just declaration
        
        // Check for function overloading
        if (currentTable->lookupFunction(sig, true)) {
            reportError("Multiply declared function: " + currentFunctionName);
            return;
        }
        
        // Check for functions with same name but different signatures (overloading)
        auto overloads = currentTable->lookupFunctions(currentFunctionName, true);
        if (!overloads.empty()) {
            reportWarning("Function overloading: " + currentFunctionName);
        }
        
        // Add function to current table
        currentTable->addSymbol(functionSymbol);
        
        // Create a new table for function scope
        std::string funcTableName = currentClassName.empty() ? "::" + currentFunctionName : currentClassName + "::" + currentFunctionName;
        auto functionTable = std::make_shared<SymbolTable>(funcTableName, currentTable.get());
        currentTable->addNestedTable(currentFunctionName, functionTable);
        
        // Add parameters to function table
        auto savedTable = currentTable;
        currentTable = functionTable;
        
        // Reprocess param list to add parameters to the function table
        ASTNode* paramListNode = signatureNode->getLeftMostChild()->getRightSibling();
        if (paramListNode && paramListNode->getNodeEnum() == NodeType::PARAM_LIST) {
            ASTNode* param = paramListNode->getLeftMostChild();
            while (param) {
                param->accept(this);
                param = param->getRightSibling();
            }
        }
        
        // Process function body (right child)
        ASTNode* bodyNode = signatureNode->getRightSibling();
        if (bodyNode) {
            bodyNode->accept(this);
        }
        
        // Restore current table
        currentTable = savedTable;
    }
}

void SymbolTableVisitor::visitFunctionBody(ASTNode* node) {
    // Process statements and local variables
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}


void SymbolTableVisitor::visitEmpty(ASTNode* node) {
    // Do nothing for empty nodes
}

void SymbolTableVisitor::visitFunctionList(ASTNode* node) {
    // Traverse all free functions in the list
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitImplementationList(ASTNode* node) {
    // Visit all implementation nodes
    ASTNode* currentChild = node->getLeftMostChild();
    while (currentChild) {
        currentChild->accept(this);
        currentChild = currentChild->getRightSibling();
    }
    currentClassName = "";
}

void SymbolTableVisitor::visitImplementation(ASTNode* node) {
    // Get implementation ID (class name)
    ASTNode* classIdNode = node->getLeftMostChild();
    if (!classIdNode) {
        reportError("Implementation missing class ID.");
        return;
    }
    
    std::string className = classIdNode->getNodeValue();
    
    // Check if the class exists in the global table
    auto classSymbol = globalTable->lookupSymbol(className);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS) {
        reportError("Implementation of undeclared class: " + className);
        return;
    }
    
    // Set current class name and table for processing member functions
    currentClassName = className;
    
    // Get the class's symbol table
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable) {
        reportError("No symbol table found for class: " + className);
        return;
    }
    
    // Save the current table and set it to the class table
    auto savedTable = currentTable;
    currentTable = classTable;
    
    // Process implementation function list (second child)
    ASTNode* funcListNode = classIdNode->getRightSibling();
    if (funcListNode) {
        funcListNode->accept(this);
    }
    
    // Restore the current table and class name
    currentTable = savedTable;
    currentClassName = "";
}

void SymbolTableVisitor::visitImplementationFunctionList(ASTNode* node) {
    // Visit all function nodes in the implementation
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        // Each child is a function node
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitMember(ASTNode* node) {
    // Either a function or variable
    ASTNode* child = node->getLeftMostChild();
    
    // Check if there's a visibility specifier
    if (child && child->getNodeEnum() == NodeType::VISIBILITY) {
        child->accept(this);
        child = child->getRightSibling();
    }
    
    // Process the actual member
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitVariable(ASTNode* node) {
    // Reset array dimensions
    currentArrayDimensions.clear();
    
    // Get the variable id node (first child)
    ASTNode* varIdNode = node->getLeftMostChild();
    if (!varIdNode) {
        reportError("Variable declaration missing identifier.");
        return;
    }
    
    std::string varName = varIdNode->getNodeValue();
    
    // Get the type node (second child)
    ASTNode* typeNode = varIdNode->getRightSibling();
    if (!typeNode) {
        reportError("Variable declaration missing type.");
        return;
    }
    typeNode->accept(this);  // This will set currentType
    
    // Check if variable is already defined in this scope
    if (currentTable->lookupSymbol(varName, true)) {
        if (currentClassName.empty()) {
            reportError("Multiply declared local variable: " + varName);
        } else {
            reportError("Multiply declared data member: " + varName + " in class " + currentClassName);
        }
        return;
    }
    
    // Create a new symbol for this variable
    auto varSymbol = std::make_shared<Symbol>(varName, currentType, SymbolKind::VARIABLE);
    varSymbol->setVisibility(currentVisibility);
    
    // Add array dimensions if any
    for (int dim : currentArrayDimensions) {
        varSymbol->addArrayDimension(dim);
    }
    
    // Add variable to current table
    currentTable->addSymbol(varSymbol);
}

void SymbolTableVisitor::visitVariableId(ASTNode* node) {
    // Just store the variable name
    // This is handled in the parent method
}

void SymbolTableVisitor::visitLocalVariable(ASTNode* node) {
    // LocalVariable only has one child, which is a Variable node
    ASTNode* variableNode = node->getLeftMostChild();
    if (!variableNode) {
        reportError("Local variable declaration missing variable node.");
        return;
    }
    
    // Process the variable node
    // The variable visitor will handle type, name, and array dimensions
    variableNode->accept(this);
    
    // The symbol was already added to the symbol table in visitVariable,
    // but we might want to set any special properties for local variables here
    // For now, there's nothing special to do
}

void SymbolTableVisitor::visitFunctionId(ASTNode* node) {
    currentFunctionName = node->getNodeValue();
}

void SymbolTableVisitor::visitFunctionSignature(ASTNode* node) {
    // Clear previous param types
    currentParamTypes.clear();

    // Get the function_id node
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (!funcIdNode) {
        reportError("Function signature missing function_id.");
        return;
    }
    // Function id node value is the function name
    currentFunctionName = funcIdNode->getNodeValue();

    // Get the param_list node
    ASTNode* paramListNode = funcIdNode->getRightSibling();
    if (!paramListNode) {
        reportWarning("Function signature missing parameter list.");
    }
    // Process parameter list if exists
    // param list will recursively call visitParam
    if (paramListNode) {
        paramListNode->accept(this);
    }

    // Get the type node
    ASTNode* typeNode = paramListNode ? paramListNode->getRightSibling() : funcIdNode->getRightSibling();
    if (!typeNode) {
        reportError("Function signature missing return type.");
        return;
    }

    // Store return type
    currentType = typeNode->getNodeValue();
}

void SymbolTableVisitor::visitConstructorSignature(ASTNode* node) {
    // Constructor has the same name as the class and returns the class type
    currentFunctionName = currentClassName;
    currentType = currentClassName;
    
    // Clear previous param types
    currentParamTypes.clear();
    
    // Process parameter list if exists
    ASTNode* paramListNode = node->getLeftMostChild();
    if (paramListNode) {
        paramListNode->accept(this);
    }
}

void SymbolTableVisitor::visitParamList(ASTNode* node) {
    // Process each parameter
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitParam(ASTNode* node) {
    // Reset array dimensions
    currentArrayDimensions.clear();
    
    // Get parameter id (first child)
    ASTNode* paramIdNode = node->getLeftMostChild();
    if (!paramIdNode) {
        reportError("Parameter missing identifier.");
        return;
    }
    
    std::string paramName = paramIdNode->getNodeValue();
    
    // Get parameter type (second child)
    ASTNode* typeNode = paramIdNode->getRightSibling();
    if (!typeNode) {
        reportError("Parameter missing type.");
        return;
    }
    
    // Check if this is an array type
    if (typeNode->getNodeEnum() == NodeType::ARRAY_TYPE) {
        // Handle array type
        ASTNode* dimNode = typeNode->getLeftMostChild();
        if (dimNode && dimNode->getNodeEnum() == NodeType::ARRAY_DIMENSION) {
            // If dimension is "dynamic", use -1 to represent a dynamic array
            if (dimNode->getNodeValue() == "dynamic") {
                currentArrayDimensions.push_back(-1);
            } else {
                // Try to parse the dimension as an integer
                try {
                    int dim = std::stoi(dimNode->getNodeValue());
                    currentArrayDimensions.push_back(dim);
                } catch (const std::exception&) {
                    reportError("Invalid array dimension: " + dimNode->getNodeValue());
                }
            }
        }
        
        // Get the base type
        ASTNode* baseTypeNode = dimNode ? dimNode->getRightSibling() : nullptr;
        if (baseTypeNode) {
            // Process base type to set currentType
            baseTypeNode->accept(this);
        } else {
            reportError("Array type missing base type.");
            return;
        }
    } else {
        // This is a normal (non-array) type
        typeNode->accept(this);
    }
    
    // Format the type with array dimensions
    std::string typeWithDimensions = currentType;
    for (int dim : currentArrayDimensions) {
        if (dim > 0) {
            typeWithDimensions += "[" + std::to_string(dim) + "]";
        } else {
            typeWithDimensions += "[]"; // For dynamic arrays
        }
    }
    
    // Add formatted type to currentParamTypes for function signature
    currentParamTypes.push_back(typeWithDimensions);
    
    // Also create a parameter symbol and add it to the current table
    if (currentTable && currentTable->getScopeName().find("::") != std::string::npos) {
        auto paramSymbol = std::make_shared<Symbol>(paramName, currentType, SymbolKind::PARAMETER);
        
        // Add array dimensions to the parameter symbol
        for (int dim : currentArrayDimensions) {
            paramSymbol->addArrayDimension(dim);
        }
        
        // Check if parameter is already defined in this scope
        if (currentTable->lookupSymbol(paramName, true)) {
            reportError("Multiply declared parameter: " + paramName);
            return;
        }
        
        // Add parameter to current table
        currentTable->addSymbol(paramSymbol);
    }
}

void SymbolTableVisitor::visitParamId(ASTNode* node) {
    // Just get the parameter name, handled in visitParam
}

void SymbolTableVisitor::visitType(ASTNode* node) {
    currentType = node->getNodeValue();
}

void SymbolTableVisitor::visitArrayType(ASTNode* node) {
    // Process base type
    ASTNode* typeNode = node->getLeftMostChild();
    typeNode->accept(this);
    
    // Process dimensions
    ASTNode* dimNode = typeNode->getRightSibling();
    while (dimNode) {
        dimNode->accept(this);
        dimNode = dimNode->getRightSibling();
    }
}

void SymbolTableVisitor::visitArrayDimension(ASTNode* node) {
    // Store the array dimension
    int dim = 0;
    try {
        dim = std::stoi(node->getNodeValue());
    } catch (...) {
        reportError("Invalid array dimension: " + node->getNodeValue());
    }
    currentArrayDimensions.push_back(dim);
}

void SymbolTableVisitor::visitBlock(ASTNode* node) {
    // Process statements in the block
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitStatementsList(ASTNode* node) {
    // Process each statement
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void SymbolTableVisitor::visitIfStatement(ASTNode* node) {
    // Process condition
    ASTNode* condNode = node->getLeftMostChild();
    if (condNode) {
        condNode->accept(this);
    }
    
    // Process then block
    ASTNode* thenNode = condNode ? condNode->getRightSibling() : nullptr;
    if (thenNode) {
        thenNode->accept(this);
    }
    
    // Process else block if exists
    ASTNode* elseNode = thenNode ? thenNode->getRightSibling() : nullptr;
    if (elseNode) {
        elseNode->accept(this);
    }
}

void SymbolTableVisitor::visitWhileStatement(ASTNode* node) {
    // Process condition
    ASTNode* condNode = node->getLeftMostChild();
    if (condNode) {
        condNode->accept(this);
    }
    
    // Process loop body
    ASTNode* bodyNode = condNode ? condNode->getRightSibling() : nullptr;
    if (bodyNode) {
        bodyNode->accept(this);
    }
}

void SymbolTableVisitor::visitCondition(ASTNode* node) {
    // Process the expression in the condition
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitSingleStatement(ASTNode* node) {
    // Process the single statement
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitExpressionStatement(ASTNode* node) {
    // Process the expression
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitAssignment(ASTNode* node) {
    // Process left side (variable)
    ASTNode* leftNode = node->getLeftMostChild();
    if (leftNode) {
        leftNode->accept(this);
    }
    
    // Process assignment operator
    ASTNode* opNode = leftNode ? leftNode->getRightSibling() : nullptr;
    if (opNode) {
        opNode->accept(this);
    }
    
    // Process right side (expression)
    ASTNode* rightNode = opNode ? opNode->getRightSibling() : nullptr;
    if (rightNode) {
        rightNode->accept(this);
    }
}

void SymbolTableVisitor::visitReadStatement(ASTNode* node) {
    // Process the variable being read
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitWriteStatement(ASTNode* node) {
    // Process the expression being written
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitReturnStatement(ASTNode* node) {
    // Process the return expression if any
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitFunctionDeclaration(ASTNode* node) {
    // The only child of a FunctionDeclaration node is the FunctionSignature node
    ASTNode* signatureNode = node->getLeftMostChild();
    if (signatureNode==nullptr) {
        reportError("Function declaration missing signature.");
        return;
    }

    // Process the function signature
    currentParamTypes.clear();
    signatureNode->accept(this);

    // Create function symbol
    auto functionSymbol = std::make_shared<Symbol>(currentFunctionName, currentType, SymbolKind::FUNCTION);
    functionSymbol->setParams(currentParamTypes);
    functionSymbol->setVisibility(currentVisibility);
    functionSymbol->setDeclared(true);

    // Check for duplicates and add to table
    FunctionSignature sig;
    sig.name = currentFunctionName;
    sig.paramTypes = currentParamTypes;

    if (currentTable->lookupFunction(sig, true)) {
        reportError("Multiply declared member function: " + currentClassName + "::" + currentFunctionName);
        return;
    }

    // Check for overloads
    auto overloads = currentTable->lookupFunctions(currentFunctionName, true);
    if (!overloads.empty()) {
        reportWarning("Function overloading: " + currentClassName + "::" + currentFunctionName);
    }

    currentTable->addSymbol(functionSymbol);
}

void SymbolTableVisitor::visitIdentifier(ASTNode* node) {
    // Just a reference to an identifier - no declaration
    // This is used in expressions, not declarations
    std::string id = node->getNodeValue();
    
    // We could check if the identifier exists, but that's typically done in a separate semantic analysis phase
}

void SymbolTableVisitor::visitSelfIdentifier(ASTNode* node) {
    // "self" refers to the current class instance
    // No need to do anything special for symbol table creation
}

void SymbolTableVisitor::visitFunctionCall(ASTNode* node) {
    // Process function name
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (funcIdNode) {
        funcIdNode->accept(this);
    }
    
    // Process arguments
    ASTNode* argsNode = funcIdNode ? funcIdNode->getRightSibling() : nullptr;
    if (argsNode) {
        argsNode->accept(this);
    }
}

void SymbolTableVisitor::visitDotIdentifier(ASTNode* node) {
    // Process object
    ASTNode* objNode = node->getLeftMostChild();
    if (objNode) {
        objNode->accept(this);
    }
    
    // Process member
    ASTNode* memberNode = objNode ? objNode->getRightSibling() : nullptr;
    if (memberNode) {
        memberNode->accept(this);
    }
}

void SymbolTableVisitor::visitArrayAccess(ASTNode* node) {
    // Process array identifier
    ASTNode* arrayNode = node->getLeftMostChild();
    if (arrayNode) {
        arrayNode->accept(this);
    }
    
    // Process index expression
    ASTNode* indexNode = arrayNode ? arrayNode->getRightSibling() : nullptr;
    if (indexNode) {
        indexNode->accept(this);
    }
}

void SymbolTableVisitor::visitRelationalExpr(ASTNode* node) {
    // Process left operand
    ASTNode* leftNode = node->getLeftMostChild();
    if (leftNode) {
        leftNode->accept(this);
    }
    
    // Process relational operator
    ASTNode* opNode = leftNode ? leftNode->getRightSibling() : nullptr;
    if (opNode) {
        opNode->accept(this);
    }
    
    // Process right operand
    ASTNode* rightNode = opNode ? opNode->getRightSibling() : nullptr;
    if (rightNode) {
        rightNode->accept(this);
    }
}

void SymbolTableVisitor::visitExpr(ASTNode* node) {
    // Process the expression
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitArithExpr(ASTNode* node) {
    // Process the term
    ASTNode* termNode = node->getLeftMostChild();
    if (termNode) {
        termNode->accept(this);
    }
    
    // Process additional terms if any (+ or -)
    ASTNode* current = termNode ? termNode->getRightSibling() : nullptr;
    while (current) {
        current->accept(this);
        current = current->getRightSibling();
    }
}

void SymbolTableVisitor::visitTerm(ASTNode* node) {
    // Process the factor
    ASTNode* factorNode = node->getLeftMostChild();
    if (factorNode) {
        factorNode->accept(this);
    }
    
    // Process additional factors if any (* or /)
    ASTNode* current = factorNode ? factorNode->getRightSibling() : nullptr;
    while (current) {
        current->accept(this);
        current = current->getRightSibling();
    }
}

void SymbolTableVisitor::visitFactor(ASTNode* node) {
    // Process the factor (identifier, literal, expression, etc.)
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitAssignOp(ASTNode* node) {
    // Assignment operator - no action needed for symbol table
}

void SymbolTableVisitor::visitRelOp(ASTNode* node) {
    // Relational operator - no action needed for symbol table
}

void SymbolTableVisitor::visitAddOp(ASTNode* node) {
    // Addition operator - no action needed for symbol table
}

void SymbolTableVisitor::visitMultOp(ASTNode* node) {
    // Multiplication operator - no action needed for symbol table
}

void SymbolTableVisitor::visitFloat(ASTNode* node) {
    // Float literal - no action needed for symbol table
}

void SymbolTableVisitor::visitInt(ASTNode* node) {
    // Integer literal - no action needed for symbol table
}

void SymbolTableVisitor::visitAttribute(ASTNode* node) {
    // Process the variable reference for attributes
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitImplementationId(ASTNode* node) {
    // Get the implementation ID (class name)
    std::string className = node->getNodeValue();
    
    // Set the current class name
    currentClassName = className;
}

void SymbolTableVisitor::checkFunctionConsistency() {
    // For each class
    for (const auto& [className, classTable] : globalTable->getNestedTables()) {
        auto classSymbol = globalTable->lookupSymbol(className);
        if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS)
            continue;
            
        // Check each function in the class
        for (const auto& [name, symbol] : classTable->getSymbols()) {
            if (symbol->getKind() != SymbolKind::FUNCTION)
                continue;
                
            if (symbol->isDeclared() && !symbol->isDefined()) {
                reportError("No definition for declared member function: " + className + "::" + name);
            }
            else if (!symbol->isDeclared() && symbol->isDefined()) {
                reportError("Definition provided for undeclared member function: " + className + "::" + name);
            }
        }
    }
}

void SymbolTableVisitor::checkShadowedMembers(const std::string& className) {
    auto classSymbol = globalTable->lookupSymbol(className);
    if (!classSymbol || classSymbol->getKind() != SymbolKind::CLASS)
        return;
        
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable)
        return;
        
    // For each inherited class
    for (const auto& parentClassName : classSymbol->getInheritedClasses()) {
        auto parentClassTable = globalTable->getNestedTable(parentClassName);
        if (!parentClassTable)
            continue;
            
        // Check each symbol in current class against parent class
        for (const auto& [name, symbol] : classTable->getSymbols()) {
            auto parentSymbol = parentClassTable->lookupSymbol(name, true);
            if (parentSymbol) {
                reportWarning("Member '" + name + "' in class '" + className + 
                              "' shadows inherited member from class '" + parentClassName + "'");
            }
        }
    }
}

void SymbolTableVisitor::outputSymbolTable(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        return;
    }
    
    writeTableToFile(outFile, globalTable);
    
    outFile.close();
}

void SymbolTableVisitor::writeTableToFile(std::ofstream& out, std::shared_ptr<SymbolTable> table, int indent) {
    std::string indentStr(indent, ' ');

    // Print table header
    out << indentStr << "===================================================================================" << std::endl;
    out << indentStr << "| table: " << table->getScopeName();
    for (int i = 0; i < 66 - table->getScopeName().length() + 7; i++) out << " ";
    out << "|" << std::endl;
    out << indentStr << "===================================================================================" << std::endl;

    // First, collect all class symbols
    std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> classSymbols;
    for (const auto& [name, symbol] : table->getSymbols()) {
        if (symbol->getKind() == SymbolKind::CLASS) {
            classSymbols.push_back({name, symbol});
        }
    }

    // Reverse the order of classes to match chronological declaration order
    std::reverse(classSymbols.begin(), classSymbols.end());

    // Print classes in reversed order
    for (const auto& [name, symbol] : classSymbols) {
        out << indentStr << "| class     | " << name;
        for (int i = 0; i < 61 - name.length() + 7; i++) out << " ";
        out << "|" << std::endl;

        // Print nested table for class
        auto nestedTable = table->getNestedTable(name);
        if (nestedTable) {
            out << indentStr << "|    ===========================================================================  |" << std::endl;
            out << indentStr << "|    | table: " << nestedTable->getScopeName();
            for (int i = 0; i < 58 - nestedTable->getScopeName().length() + 7; i++) out << " ";
            out << "|  |" << std::endl;
            out << indentStr << "|    ===========================================================================  |" << std::endl;

            // Print inheritance
            auto classSymbol = table->lookupSymbol(name);
            const auto& inherited = classSymbol->getInheritedClasses();
            out << indentStr << "|    | inherit   | ";
            if (inherited.empty()) {
                out << "none";
                for (int i = 0; i < 56; i++) out << " ";
            } else {
                out << inherited[0];
                for (int i = 0; i < 60 - inherited[0].length(); i++) out << " ";
            }
            out << "|  |" << std::endl;

            // Collect data members and functions separately
            std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> dataMembers;
            std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> memberFunctions;

            for (const auto& [memberName, memberSymbol] : nestedTable->getSymbols()) {
                if (memberSymbol->getKind() == SymbolKind::VARIABLE) {
                    dataMembers.push_back({memberName, memberSymbol});
                } else if (memberSymbol->getKind() == SymbolKind::FUNCTION) {
                    memberFunctions.push_back({memberName, memberSymbol});
                }
            }

            // Reverse data members to match declaration order
            std::reverse(dataMembers.begin(), dataMembers.end());
            
            // Reverse member functions to match declaration order
            std::reverse(memberFunctions.begin(), memberFunctions.end());

            // Print data members first
            for (const auto& [memberName, memberSymbol] : dataMembers) {
                out << indentStr << "|    | data      | " << memberName;
                for (int i = 0; i < 10 - memberName.length(); i++) out << " ";
                out << "| " << formatTypeWithDimensions(memberSymbol);
                for (int i = 0; i < 33 - memberSymbol->getType().length(); i++) out << " ";
                out << "| " << (memberSymbol->getVisibility() == Visibility::PUBLIC ? "public" : "private");
                for (int i = 0; i < 6; i++) out << " ";
                out << "|  |" << std::endl;
            }

            // Then print member functions
            for (const auto& [memberName, memberSymbol] : memberFunctions) {
                out << indentStr << "|    | function  | " << memberName;
                for (int i = 0; i < 10 - memberName.length(); i++) out << " ";
                out << "| (" << formattedParamList(memberSymbol->getParams()) << "):" << memberSymbol->getType();
                std::string funcSig = "(" + formattedParamList(memberSymbol->getParams()) + "):" + memberSymbol->getType();
                for (int i = 0; i < 33 - funcSig.length(); i++) out << " ";
                out << "| " << (memberSymbol->getVisibility() == Visibility::PUBLIC ? "public" : "private");
                for (int i = 0; i < 13-(memberSymbol->getVisibility() == Visibility::PUBLIC ? 6 : 7); i++) out << " ";
                out << "|  |" << std::endl;

                // Print function nested table
                auto funcTable = nestedTable->getNestedTable(memberName);
                if (funcTable) {
                    out << indentStr << "|    |     ==================================================================  |  |" << std::endl;
                    out << indentStr << "|    |     | table: " << funcTable->getScopeName();
                    for (int i = 0; i < 50 - funcTable->getScopeName().length() + 6; i++) out << " ";
                    out << "|  |  |" << std::endl;
                    out << indentStr << "|    |     ==================================================================  |  |" << std::endl;

                    // Collect parameters and local variables separately
                    std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> params;
                    std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> locals;

                    for (const auto& [paramName, paramSymbol] : funcTable->getSymbols()) {
                        if (paramSymbol->getKind() == SymbolKind::PARAMETER) {
                            params.push_back({paramName, paramSymbol});
                        } else if (paramSymbol->getKind() == SymbolKind::VARIABLE) {
                            locals.push_back({paramName, paramSymbol});
                        }
                    }
                    
                    // Reverse parameters and locals to match declaration order
                    std::reverse(params.begin(), params.end());
                    std::reverse(locals.begin(), locals.end());

                    // Print parameters first
                    for (const auto& [paramName, paramSymbol] : params) {
                        out << indentStr << "|    |     | param";
                        for (int i = 0; i < 4 ; i++) out << " ";
                        out << " | " << paramName;
                        for (int i = 0; i < 12 - paramName.length(); i++) out << " ";
                        out << "| " << formatTypeWithDimensions(paramSymbol);
                        for (int i = 0; i < 37 - formatTypeWithDimensions(paramSymbol).length(); i++) out << " ";
                        out << "|  |  |" << std::endl;
                    }

                    // Then print local variables
                    for (const auto& [localName, localSymbol] : locals) {
                        out << indentStr << "|    |     | local";
                        for (int i = 0; i < 4; i++) out << " ";
                        out << " | " << localName;
                        for (int i = 0; i < 12 - localName.length(); i++) out << " ";
                        out << "| " << formatTypeWithDimensions(localSymbol);
                        for (int i = 0; i < 37 - formatTypeWithDimensions(localSymbol).length(); i++) out << " ";
                        out << "|  |  |" << std::endl;
                    }

                    out << indentStr << "|    |     ==================================================================  |  |" << std::endl;
                }
            }

            out << indentStr << "|    ==========================================================================|  |" << std::endl;
        }
    }

    // Print free functions at global level
    if (table->getScopeName() == "global") {
        // Collect global functions
        std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> globalFunctions;
        for (const auto& [name, symbol] : table->getSymbols()) {
            if (symbol->getKind() == SymbolKind::FUNCTION) {
                globalFunctions.push_back({name, symbol});
            }
        }
        
        // Reverse global functions to match declaration order
        std::reverse(globalFunctions.begin(), globalFunctions.end());

        // Print global functions
        for (const auto& [name, symbol] : globalFunctions) {
            out << indentStr << "| function    | " << name;
            for (int i = 0; i < 10 - name.length() + 7; i++) out << " ";
            out << "| (" << formattedParamList(symbol->getParams()) << "):" << symbol->getType();
            std::string funcSig = "(" + formattedParamList(symbol->getParams()) + "):" + symbol->getType();
            for (int i = 0; i < 33 - funcSig.length() + 14; i++) out << " ";
            out << "|" << std::endl;

            // Print function nested table
            auto funcTable = table->getNestedTable(name);
            if (funcTable) {
                out << indentStr << "|    ===========================================================================  |" << std::endl;
                out << indentStr << "|    | table: " << funcTable->getScopeName();
                for (int i = 0; i < 58 - funcTable->getScopeName().length() + 7; i++) out << " ";
                out << "|  |" << std::endl;
                out << indentStr << "|    ===========================================================================  |" << std::endl;

                // Collect parameters and local variables separately
                std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> params;
                std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> locals;

                for (const auto& [paramName, paramSymbol] : funcTable->getSymbols()) {
                    if (paramSymbol->getKind() == SymbolKind::PARAMETER) {
                        params.push_back({paramName, paramSymbol});
                    } else if (paramSymbol->getKind() == SymbolKind::VARIABLE) {
                        locals.push_back({paramName, paramSymbol});
                    }
                }
                
                // Reverse parameters and locals to match declaration order
                std::reverse(params.begin(), params.end());
                std::reverse(locals.begin(), locals.end());

                // Print parameters first
                for (const auto& [paramName, paramSymbol] : params) {
                    out << indentStr << "|    | param      | " << paramName;
                    for (int i = 0; i < 12 - paramName.length() + 5; i++) out << " ";
                    out << "| " << formatTypeWithDimensions(paramSymbol);
                    for (int i = 0; i < 33 - formatTypeWithDimensions(paramSymbol).length() + 7; i++) out << " ";
                    out << "|  |" << std::endl;
                }

                // Then print local variables
                for (const auto& [localName, localSymbol] : locals) {
                    out << indentStr << "|    | local      | " << localName;
                    for (int i = 0; i < 12 - localName.length() + 5; i++) out << " ";
                    out << "| " << formatTypeWithDimensions(localSymbol);
                    for (int i = 0; i < 33 - formatTypeWithDimensions(localSymbol).length() + 7; i++) out << " ";
                    out << "|  |" << std::endl;
                }

                out << indentStr << "|    ===========================================================================  |" << std::endl;
            }
        }
    }

    out << indentStr << "===================================================================================" << std::endl;
}

// Helper to format parameter lists
std::string SymbolTableVisitor::formattedParamList(const std::vector<std::string>& params) {
    std::stringstream ss;
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << params[i];
    }
    return ss.str();
}

std::string SymbolTableVisitor::formatTypeWithDimensions(const std::shared_ptr<Symbol>& symbol) const {
    std::string type = symbol->getType();
    const auto& dimensions = symbol->getArrayDimensions();
    for (int dim : dimensions) {
        if (dim > 0) {
            type += "[" + std::to_string(dim) + "]";
        } else {
            type += "[]"; // For arrays without explicit size
        }
    }
    return type;
}
