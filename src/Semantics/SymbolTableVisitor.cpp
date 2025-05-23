#include "SymbolTableVisitor.h"
#include <iostream>
#include <sstream>
#include <algorithm> 


// Equality comparison
bool FunctionSignature::operator==(const FunctionSignature& other) const {
    // Names must match
    if (name != other.name) {
        return false;
    }
    
    // Parameter counts must match
    if (paramTypes.size() != other.paramTypes.size()) {
        return false;
    }
    
    // Each parameter type must match in order
    for (size_t i = 0; i < paramTypes.size(); i++) {
        if (paramTypes[i] != other.paramTypes[i]) {
            return false;
        }
    }
    
    return true;
}

// Also implement the hash function for unordered maps
std::size_t std::hash<FunctionSignature>::operator()(const FunctionSignature& sig) const {
    std::size_t h = std::hash<std::string>{}(sig.name);
    
    // Combine hash with parameter types
    for (const auto& paramType : sig.paramTypes) {
        h ^= std::hash<std::string>{}(paramType) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    
    return h;
}

// Symbol implementation
Symbol::Symbol(const std::string& name, const std::string& type, SymbolKind kind)
    : name(name), type(type), kind(kind) {}
// Copy constructor implementation
Symbol::Symbol(const Symbol& other)
    : name(other.name),
      type(other.type),
      kind(other.kind),
      visibility(other.visibility),
      inheritedClasses(other.inheritedClasses),
      params(other.params),
      defined(other.defined),
      declared(other.declared),
      arrayDimensions(other.arrayDimensions),
      declarationLine(other.declarationLine),
      definitionLine(other.definitionLine),
      metadata(other.metadata)
{
    // All member variables copied directly
}
// SymbolTable implementation
SymbolTable::SymbolTable(const std::string& scopeName, SymbolTable* parent)
    : scopeName(scopeName), parent(parent) {}

// Deep copy constructor implementation
SymbolTable::SymbolTable(const SymbolTable& other)
    : scopeName(other.scopeName), 
      parent(other.parent),
      symbolInsertionOrder(other.symbolInsertionOrder),
      metadata(other.metadata),
      nestedTableInsertionOrder(other.nestedTableInsertionOrder)
{
    // Deep copy all symbols
    for (const auto& [name, symbol] : other.symbols) {
        auto symbolCopy = std::make_shared<Symbol>(*symbol); // Copy the Symbol object
        symbols[name] = symbolCopy;
    }
    
    // Deep copy the function symbol if it exists
    if (other.functionSymbol) {
        functionSymbol = std::make_shared<Symbol>(*other.functionSymbol);
    }
    
    // Deep copy the functions map
    for (const auto& [sig, symbol] : other.functions) {
        // Find the corresponding symbol in our new symbols map
        auto it = symbols.find(symbol->getName());
        if (it != symbols.end()) {
            functions[sig] = it->second;
        } else {
            // If not in symbols (unlikely), create a new copy
            functions[sig] = std::make_shared<Symbol>(*symbol);
        }
    }
    
    // Deep copy nested tables (recursively)
    for (const auto& [name, nestedTable] : other.nestedTables) {
        auto tableCopy = std::make_shared<SymbolTable>(*nestedTable); // Recursive copy
        tableCopy->parent = this; // Update parent pointer to point to this table
        nestedTables[name] = tableCopy;
    }
}
    
bool SymbolTable::addSymbol(std::shared_ptr<Symbol> symbol) {
    const std::string& name = symbol->getName();
    
    // Special case for functions to allow overloading
    if (symbol->getKind() == SymbolKind::FUNCTION) {
        FunctionSignature sig;
        sig.name = name;
        sig.paramTypes = symbol->getParams();
        
        // Check if this exact signature already exists
        if (functions.find(sig) != functions.end()) {
            return false; // Exact function signature already exists
        }
        
        // Add to the functions map by signature
        functions[sig] = symbol;
        
        // For functions, we allow multiple with the same name (overloading)
        // Replace in the symbols map or add if not exists
        symbols[name] = symbol;
        
        // Track insertion order
        if (std::find(symbolInsertionOrder.begin(), symbolInsertionOrder.end(), name) == symbolInsertionOrder.end()) {
            symbolInsertionOrder.push_back(name);
        }
        
        return true;
    }
    
    // For non-functions, check if the name already exists
    if (symbols.find(name) != symbols.end()) {
        return false; // Symbol already exists
    }
    
    // Add to symbols map
    symbols[name] = symbol;
    // Track insertion order
    symbolInsertionOrder.push_back(name);
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
    
    // Look in the functions map for all functions with this name
    for (const auto& [sig, symbol] : functions) {
        if (sig.name == name) {
            result.push_back(symbol);
        }
    }
    
    // Check parent scope if not local-only search
    if (!localOnly && parent) {
        auto parentResults = parent->lookupFunctions(name);
        result.insert(result.end(), parentResults.begin(), parentResults.end());
    }
    
    return result;
}

void SymbolTable::addNestedTable(const std::string& name, std::shared_ptr<SymbolTable> table) {
    nestedTables[name] = table;
    nestedTableInsertionOrder.push_back(name);
}

std::shared_ptr<SymbolTable> SymbolTable::getNestedTable(const std::string& name) {
    auto it = nestedTables.find(name);
    if (it != nestedTables.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::pair<FunctionSignature, std::shared_ptr<Symbol>>> SymbolTable::getAllFunctions() const {
    std::vector<std::pair<FunctionSignature, std::shared_ptr<Symbol>>> result;
    for (const auto& [sig, symbol] : functions) {
        result.push_back({sig, symbol});
    }
    return result;
}

bool SymbolTable::removeSymbol(const std::string& name) {
    // First check if the symbol exists
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        return false; // Symbol not found
    }
    
    // Remove from the symbols map
    symbols.erase(it);
    
    // Remove from the insertion order vector
    auto orderIt = std::find(symbolInsertionOrder.begin(), symbolInsertionOrder.end(), name);
    if (orderIt != symbolInsertionOrder.end()) {
        symbolInsertionOrder.erase(orderIt);
    }
    
    return true;
}

// SymbolTableVisitor implementation
SymbolTableVisitor::SymbolTableVisitor() {
    // Initialize with empty tables and state
}

SymbolTableVisitor::~SymbolTableVisitor() {
    // Clean up if needed
}

// Implement updated error reporting methods
void SymbolTableVisitor::reportError(const std::string& message, ASTNode* node) {
    int line = node ? node->getLineNumber() : 0;
    errors.push_back({("SymTab Error: " + message), line});
    std::cerr << " SymTab Error" << (line > 0 ? " at line " + std::to_string(line) : "") 
              << ": " << message << std::endl;
}

void SymbolTableVisitor::reportError(const std::string& message, int line) {
    errors.push_back({("SymTab Error: " + message), line});
    std::cerr << " SymTab Error" << (line > 0 ? " at line " + std::to_string(line) : "") 
              << ": " << message << std::endl;
}

void SymbolTableVisitor::reportWarning(const std::string& message, ASTNode* node) {
    int line = node ? node->getLineNumber() : 0;
    warnings.push_back({("SymTab Warning: " + message), line});
    std::cerr << "SymTab Warning" << (line > 0 ? " at line " + std::to_string(line) : "") 
              << ": " << message << std::endl;
}

void SymbolTableVisitor::reportWarning(const std::string& message, int line) {
    warnings.push_back({("SymTab Warning: " + message), line});
    std::cerr << "SymTab Warning" << (line > 0 ? " at line " + std::to_string(line) : "") 
              << ": " << message << std::endl;
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
        reportError("Multiple declared class: " + currentClassName, node);
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
        reportError("Class " + currentClassName + " inherits from undefined class: " + inheritedClass, node);
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
        reportError("Function missing signature.", node);
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
                      currentClassName + "::" + currentFunctionName, node);
            return;
        }
        
        // Mark the function as defined
        existingFunction->setDefined(true);
        existingFunction->setDefinitionLine(node->getLineNumber()); // Store definition line
        
        // Create UNIQUE key for this function that includes parameter types
        std::string uniqueKey = currentFunctionName;
        if (!currentParamTypes.empty()) {
            uniqueKey += "_" + formatParamTypesForTableName(currentParamTypes);
        }
        
        // Create or get the function's symbol table using the unique key
        std::string funcTableName = currentClassName + "::" + currentFunctionName;
        auto functionTable = currentTable->getNestedTable(uniqueKey);
        if (!functionTable) {
            // Create a new table if one doesn't exist yet
            functionTable = std::make_shared<SymbolTable>(funcTableName, currentTable.get());
            currentTable->addNestedTable(uniqueKey, functionTable);
            functionTable->setFunctionSymbol(existingFunction);
        }
        
        // Store the function symbol with this subtable for overload resolution
        functionTable->setFunctionSymbol(existingFunction);
        
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
        functionSymbol->setParams(currentParamNames, currentParamTypes);
        functionSymbol->setVisibility(currentVisibility);
        functionSymbol->setDeclared(true);
        functionSymbol->setDefined(true);  // Since this is a function definition
        
        // Check for function overloading
        if (currentTable->lookupFunction(sig, true)) {
            reportError("Multiple declared function: " + currentFunctionName, node);
            return;
        }
        
        // Check for functions with same name but different signatures (overloading)
        auto overloads = currentTable->lookupFunctions(currentFunctionName, true);
        if (!overloads.empty()) {
            reportWarning("Function overloading: " + currentFunctionName, node);
        }
        
        // Add function to current table
        currentTable->addSymbol(functionSymbol);
        
        // Create unique key for storing function table
        std::string uniqueKey = currentFunctionName;
        if (!currentParamTypes.empty()) {
            uniqueKey += "_" + formatParamTypesForTableName(currentParamTypes);
        }

        // Use uniqueKey for lookups and storage
        auto functionTable = currentTable->getNestedTable(uniqueKey);
        if (!functionTable) {
            // Create function table with consistent naming for display
            std::string funcTableName = (currentClassName.empty() ? "::" : currentClassName + "::") + currentFunctionName;
            functionTable = std::make_shared<SymbolTable>(funcTableName, currentTable.get());
            currentTable->addNestedTable(uniqueKey, functionTable);
            functionTable->setFunctionSymbol(functionSymbol);
        }
        
        // Change scope to function table
        auto savedTable = currentTable;
        currentTable = functionTable;
        
        // Process parameters
        ASTNode* paramListNode = signatureNode->getLeftMostChild()->getRightSibling();
        if (paramListNode && paramListNode->getNodeEnum() == NodeType::PARAM_LIST) {
            ASTNode* param = paramListNode->getLeftMostChild();
            while (param) {
                param->accept(this);
                param = param->getRightSibling();
            }
        }
        
        // Process function body - this adds local variables
        ASTNode* bodyNode = signatureNode->getRightSibling();
        if (bodyNode) {
            bodyNode->accept(this);
        }
        
        // Restore scope
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
    
    // Set current class name and table for processing member functions
    currentClassName = className;
    
    // Get the class's symbol table
    auto classTable = globalTable->getNestedTable(className);
    if (!classTable) {
        reportError("No symbol table found for class: " + className, node);
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
    // Either a function, attribute (variable), or visibility specifier
    ASTNode* child = node->getLeftMostChild();

    // Check if there's a visibility specifier first
    if (child && child->getNodeEnum() == NodeType::VISIBILITY) {
        child->accept(this); // Sets currentVisibility
        child = child->getRightSibling(); // Move to the actual member
    } else {
        // Reset to default visibility if not specified for this member
        // Assuming default is private based on your visitClass logic
        currentVisibility = Visibility::PRIVATE;
    }

    // Process the actual member (FunctionDeclaration or Attribute)
    if (child) {
        // The child node (FunctionDeclaration or Attribute) will handle
        // creating the symbol and setting its visibility based on currentVisibility.
        child->accept(this);
    }
}

void SymbolTableVisitor::visitVariable(ASTNode* node) {
    // Reset array dimensions for this variable
    currentArrayDimensions.clear();

    // Child 1: Variable ID
    ASTNode* varIdNode = node->getLeftMostChild();
    if (!varIdNode || varIdNode->getNodeEnum() != NodeType::VARIABLE_ID) {
        reportError("Variable declaration missing identifier.", node);
        return;
    }
    std::string varName = varIdNode->getNodeValue();

    // Child 2: Type
    ASTNode* typeNode = varIdNode->getRightSibling();
    if (!typeNode || typeNode->getNodeEnum() != NodeType::TYPE) {
        reportError("Variable declaration missing type.", node);
        return;
    }
    typeNode->accept(this); // Sets currentType

    // Child 3: Dimension List (optional)
    ASTNode* dimListNode = typeNode->getRightSibling();
    bool isArray = false;
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        // Check if the dimlist actually has dimensions
        if (dimListNode->getLeftMostChild()) {
            isArray = true;
            dimListNode->accept(this); // Populates currentArrayDimensions
        }
    } else if (dimListNode) {
         reportError("Expected DimList node after type in variable declaration.", node);
         return; // Or handle as non-array? Depends on grammar strictness.
    }
    // If dimListNode is null, it's definitely not an array.

    // Check if variable is already defined in this scope
    if (currentTable->lookupSymbol(varName, true)) {
        if (currentClassName.empty() || currentTable->getScopeName().find("::") != std::string::npos) { // Check if in function or global
             reportError("Multiple declared local variable: " + varName, node);
        } else { // Must be a class member
             reportError("Multiple declared data member: " + varName + " in class " + currentClassName, node);
        }
        return;
    }

    // Create a new symbol for this variable
    // Use the base type for the symbol's type field
    auto varSymbol = std::make_shared<Symbol>(varName, currentType, SymbolKind::VARIABLE);
    varSymbol->setVisibility(currentVisibility); // Visibility set by parent (Member or default)
    varSymbol->setDeclarationLine(node->getLineNumber()); // Store line number

    // Add array dimensions if it was an array
    if (isArray) {
        for (int dim : currentArrayDimensions) {
            varSymbol->addArrayDimension(dim);
        }
    }

    // Add variable to current table
    currentTable->addSymbol(varSymbol);
}

void SymbolTableVisitor::visitVariableId(ASTNode* node) {
    // Just store the variable name
    // This is handled in the parent method
}

void SymbolTableVisitor::visitLocalVariable(ASTNode* node) {
    // The only child should be a Variable node
    ASTNode* variableNode = node->getLeftMostChild();
    if (variableNode && variableNode->getNodeEnum() == NodeType::VARIABLE) {
        // Process the underlying variable declaration
        variableNode->accept(this);
    } else {
        reportError("LocalVariable node expects a Variable node as its child.", node);
    }
}

void SymbolTableVisitor::visitFunctionId(ASTNode* node) {
    currentFunctionName = node->getNodeValue();
}

void SymbolTableVisitor::visitFunctionSignature(ASTNode* node) {
    // Clear previous param types and names
    currentParamTypes.clear();
    currentParamNames.clear();

    // Get the function_id node
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (!funcIdNode) {
        reportError("Function signature missing function_id.", node);
        return;
    }
    // Function id node value is the function name
    currentFunctionName = funcIdNode->getNodeValue();

    // Get the param_list node
    ASTNode* paramListNode = funcIdNode->getRightSibling();
    if (!paramListNode) {
        reportWarning("Function signature missing parameter list.", node);
    }
    // Process parameter list if exists
    // param list will recursively call visitParam
    if (paramListNode) {
        paramListNode->accept(this);
    }

    // Get the type node
    ASTNode* typeNode = paramListNode ? paramListNode->getRightSibling() : funcIdNode->getRightSibling();
    if (!typeNode) {
        reportError("Function signature missing return type.", node);
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
    // Reset array dimensions for this parameter
    currentArrayDimensions.clear();

    // Child 1: Parameter ID
    ASTNode* paramIdNode = node->getLeftMostChild();
    if (!paramIdNode || paramIdNode->getNodeEnum() != NodeType::PARAM_ID) {
        reportError("Parameter missing identifier.", node);
        return;
    }
    std::string paramName = paramIdNode->getNodeValue();

    // Child 2: Type
    ASTNode* typeNode = paramIdNode->getRightSibling();
    if (!typeNode || typeNode->getNodeEnum() != NodeType::TYPE) {
        reportError("Parameter missing type.", node);
        return;
    }
    typeNode->accept(this); // Sets currentType

    // Child 3: Dimension List (optional)
    ASTNode* dimListNode = typeNode->getRightSibling();
    bool isArray = false;
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        if (dimListNode->getLeftMostChild()) {
            isArray = true;
            dimListNode->accept(this); // Populates currentArrayDimensions
        }
    } else if (dimListNode) {
         reportError("Expected DimList node after type in parameter declaration.", node);
         // Decide how to proceed, maybe treat as non-array
    }
    // If dimListNode is null, it's not an array.

    // Store parameter name for function signature
    currentParamNames.push_back(paramName);

    // Format the type string including dimensions for the signature
    std::string typeWithDimensions = currentType;
    if (isArray) {
        // Use the helper to format correctly
        // Create a temporary symbol just for formatting helper
        auto tempSymbol = std::make_shared<Symbol>("temp", currentType, SymbolKind::PARAMETER);
        for(int dim : currentArrayDimensions) tempSymbol->addArrayDimension(dim);
        typeWithDimensions = formatTypeWithDimensions(tempSymbol); // Use helper
    }

    // Add formatted type to currentParamTypes for function signature matching
    currentParamTypes.push_back(typeWithDimensions);

    // Add parameter symbol to the function's symbol table
    // This should happen when processing the function body or declaration context
    if (currentTable && currentTable->getScopeName().find("::") != std::string::npos) { // Check if inside a function table
        // Determine if this is a function implementation or declaration
        bool isImplementation = false;
        ASTNode* funcNode = node->getParent();  // PARAM_LIST
        if (funcNode) funcNode = funcNode->getParent();  // FUNCTION_SIGNATURE
        if (funcNode) funcNode = funcNode->getParent();  // FUNCTION
        if (funcNode && funcNode->getParent()) {
            ASTNode* parentOfFunc = funcNode->getParent();
            if (parentOfFunc->getNodeEnum() == NodeType::IMPLEMENTATION_FUNCTION_LIST) {
                isImplementation = true;
            }
        }

        // For implementations, we don't report duplicate parameters since they were
        // already added during the class definition phase
        if (currentTable->lookupSymbol(paramName, true)) {
            if (!isImplementation) {
                reportError("Multiple declared parameter: " + paramName, node);
                return;
            }
            // For implementation, just return without error - parameter already exists
            return;
        }

        // Create the symbol using the formatted type string
        auto paramSymbol = std::make_shared<Symbol>(paramName, typeWithDimensions, SymbolKind::PARAMETER);

        // Add array dimensions to the symbol itself if it's an array
        if (isArray) {
            for (int dim : currentArrayDimensions) {
                paramSymbol->addArrayDimension(dim);
            }
        }

        // Add parameter symbol to the current function's table
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
    // Clear existing array dimensions
    currentArrayDimensions.clear();
    
    // Process base type (first child)
    ASTNode* typeNode = node->getLeftMostChild();
    if (typeNode) {
        typeNode->accept(this);
    }
    
    // Get dimension list node (second child)
    ASTNode* dimListNode = typeNode ? typeNode->getRightSibling() : nullptr;
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        // Process dimension list to collect all dimensions
        dimListNode->accept(this);
    } else {
        // For backward compatibility - handle direct dimension nodes
        // This can be removed once all code is updated
        ASTNode* dimNode = typeNode ? typeNode->getRightSibling() : nullptr;
        while (dimNode) {
            dimNode->accept(this);
            dimNode = dimNode->getRightSibling();
        }
    }
}
void SymbolTableVisitor::visitArrayDimension(ASTNode* node) {
    std::string dimensionValue = node->getNodeValue();
    
    // Check if this is a dynamic array (marked as "dynamic" or empty)
    if (dimensionValue == "dynamic" || dimensionValue.empty()) {
        // Dynamic array - use special marker -1
        currentArrayDimensions.push_back(-1);
        return;
    }
    
    // For regular arrays with dimension values stored directly in the node
    try {
        int dimension = std::stoi(dimensionValue);
        if (dimension <= 0) {
            reportError("Array dimension must be positive.", node);
        } else {
            currentArrayDimensions.push_back(dimension);
        }
    } catch (const std::exception&) {
        reportError("Invalid array dimension: " + dimensionValue, node);
        currentArrayDimensions.push_back(-1);  // Default to dynamic
    }
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
        reportError("Function declaration missing signature.", node);
        return;
    }

    // Process the function signature
    currentParamTypes.clear();
    signatureNode->accept(this);

    // Create function signature for lookup
    FunctionSignature sig;
    sig.name = currentFunctionName;
    sig.paramTypes = currentParamTypes;

    // Check for duplicates (exact signature match)
    if (currentTable->lookupFunction(sig, true)) {
        reportError("Multiple declared member function: " + currentClassName + "::" + currentFunctionName, node);
        return;
    }

    // Check for overloads (same name, different parameters)
    auto overloads = currentTable->lookupFunctions(currentFunctionName, true);
    if (!overloads.empty()) {
        reportWarning("Function overloading: " + currentClassName + "::" + currentFunctionName, node);
    }

    // Create function symbol
    auto functionSymbol = std::make_shared<Symbol>(currentFunctionName, currentType, SymbolKind::FUNCTION);
    functionSymbol->setParams(currentParamNames, currentParamTypes);  // Use new method
    functionSymbol->setVisibility(currentVisibility);
    functionSymbol->setDeclared(true);
    functionSymbol->setDeclarationLine(node->getLineNumber()); // Store line number

    // Add function to current table
    currentTable->addSymbol(functionSymbol);

    // For class member functions (in visitFunctionDeclaration):
    // After adding the function symbol to currentTable:
    createFunctionSubtable(currentTable, currentClassName, currentFunctionName, functionSymbol);
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
    // // Process object
    // ASTNode* objNode = node->getLeftMostChild();
    // if (objNode) {
    //     objNode->accept(this);
    // }
    
    // // Process member
    // ASTNode* memberNode = objNode ? objNode->getRightSibling() : nullptr;
    // if (memberNode) {
    //     memberNode->accept(this);
    // }
}

void SymbolTableVisitor::visitDotAccess(ASTNode* node) {
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
    // The only child should be a Variable node
    ASTNode* variableNode = node->getLeftMostChild();
    if (variableNode && variableNode->getNodeEnum() == NodeType::VARIABLE) {
        // Process the underlying variable declaration
        // Visibility is typically set before calling accept on the Member node containing this Attribute
        variableNode->accept(this);
    } else {
        reportError("Attribute node expects a Variable node as its child.", node);
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
                reportError("No definition for declared member function: " + className + "::" + name, 
                            symbol->getDeclarationLine()); // Use declaration line
            }
            else if (!symbol->isDeclared() && symbol->isDefined()) {
                reportError("Definition provided for undeclared member function: " + className + "::" + name,
                            symbol->getDefinitionLine()); // Use definition line
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
                              "' shadows inherited member from class '" + parentClassName + "'", 
                              symbol->getDeclarationLine());
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
    int spaces = 66 - table->getScopeName().length() + 7;
    if (spaces > 0) {
        for (int i = 0; i < spaces; i++) out << " ";
    }
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
        spaces = 61 - name.length() + 7;
        if (spaces > 0) {
            for (int i = 0; i < spaces; i++) out << " ";
        }
        out << "|" << std::endl;

        // Print nested table for class
        auto nestedTable = table->getNestedTable(name);
        if (nestedTable) {
            out << indentStr << "|    ===========================================================================  |" << std::endl;
            out << indentStr << "|    | table: " << nestedTable->getScopeName();
            spaces = 58 - nestedTable->getScopeName().length() + 7;
            if (spaces > 0) {
                for (int i = 0; i < spaces; i++) out << " ";
            }
            out << "|  |" << std::endl;
            out << indentStr << "|    ===========================================================================  |" << std::endl;

            // Print inheritance
            auto classSymbol = table->lookupSymbol(name);
            const auto& inherited = classSymbol->getInheritedClasses();
            out << indentStr << "|    | inherit   | ";
            if (inherited.empty()) {
                out << "none";
                if (56 > 0) {  // Using a constant for clarity
                    for (int i = 0; i < 56; i++) out << " ";
                }
            } else {
                out << inherited[0];
                spaces = 60 - inherited[0].length();
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
            }
            out << "|  |" << std::endl;

            // Collect data members separately
            std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> dataMembers;

            for (const auto& [memberName, memberSymbol] : nestedTable->getSymbols()) {
                if (memberSymbol->getKind() == SymbolKind::VARIABLE) {
                    dataMembers.push_back({memberName, memberSymbol});
                }
            }

            // Collect member functions including all overloads
            std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> memberFunctions;
            auto allFunctions = nestedTable->getAllFunctions();
            for (const auto& [sig, symbol] : allFunctions) {
                // Only include functions defined in this class (not inherited)
                if (nestedTable->lookupSymbol(sig.name, true)) {
                    memberFunctions.push_back({sig.name, symbol});
                }
            }

            // Reverse data members to match declaration order
            std::reverse(dataMembers.begin(), dataMembers.end());

            // Reverse member functions to match declaration order 
            std::reverse(memberFunctions.begin(), memberFunctions.end());

            // Print data members first
            for (const auto& [memberName, memberSymbol] : dataMembers) {
                out << indentStr << "|    | data      | " << memberName;
                spaces = 20 - memberName.length();
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
                out << "| " << formatTypeWithDimensions(memberSymbol);
                spaces = 23 - formatTypeWithDimensions(memberSymbol).length();
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
                out << "| " << (memberSymbol->getVisibility() == Visibility::PUBLIC ? "public" : "private");
                if (6 > 0) {  // Using a constant for clarity
                    for (int i = 0; i < 6; i++) out << " ";
                }
                out << "|  |" << std::endl;
            }

            // Then print member functions
            for (const auto& [memberName, memberSymbol] : memberFunctions) {
                out << indentStr << "|    | function  | " << memberName;
                int spaces = 10 - memberName.length();
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
                
                // Format params list
                std::string paramList = formatFunctionParams(memberSymbol);
                out << "| " << paramList;
                spaces = 33 - paramList.length();
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
                out << "| " << (memberSymbol->getVisibility() == Visibility::PUBLIC ? "public" : "private");
                spaces = 13 - (memberSymbol->getVisibility() == Visibility::PUBLIC ? 6 : 7);
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
                out << "|  |" << std::endl;
                
                // Immediately print this function's parameter table
                out << indentStr << "|    |     ==================================================================  |  |" << std::endl;
                out << indentStr << "|    |     | table: " << nestedTable->getScopeName() << "::" << memberName;
                spaces = 56 - (nestedTable->getScopeName() + "::" + memberName).length();
                if (spaces > 0) {
                    for (int i = 0; i < spaces; i++) out << " ";
                }
                out << "|  |  |" << std::endl;
                
                out << indentStr << "|    |     ==================================================================  |  |" << std::endl;
                
                // Print this function's parameters
                const auto& paramInfo = memberSymbol->getParamInfo();
                for (const auto& param : paramInfo) {
                    out << indentStr << "|    |     | param     | " << param.name;
                    spaces = 12 - param.name.length();
                    if (spaces > 0) {
                        for (int i = 0; i < spaces; i++) out << " ";
                    }
                    out << "| " << param.type;
                    spaces = 37 - param.type.length();
                    if (spaces > 0) {
                        for (int i = 0; i < spaces; i++) out << " ";
                    }
                    out << "|  |  |" << std::endl;
                }
                
                // If this function has local variables, print them too
                // Use unique key based on function name + parameter types to find the right subtable
                std::string uniqueKey = memberName;
                const auto& paramTypes = memberSymbol->getParams();
                if (!paramTypes.empty()) {
                    uniqueKey += "_" + formatParamTypesForTableName(paramTypes);
                }

                auto functionTable = nestedTable->getNestedTable(uniqueKey);
                if (functionTable) {
                    // Collect and reverse local variables to match declaration order
                    std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> localVars;
                    for (const auto& [localName, localSymbol] : functionTable->getSymbols()) {
                        if (localSymbol->getKind() == SymbolKind::VARIABLE) {
                            localVars.push_back({localName, localSymbol});
                        }
                    }
                    std::reverse(localVars.begin(), localVars.end());
                    for (const auto& [localName, localSymbol] : localVars) {
                        if (localSymbol->getKind() == SymbolKind::VARIABLE) { // Local variables
                            out << indentStr << "|    |     | local     | " << localName;
                            spaces = 12 - localName.length();
                            if (spaces > 0) {
                                for (int i = 0; i < spaces; i++) out << " ";
                            }
                            std::string formattedType = formatTypeWithDimensions(localSymbol);
                            out << "| " << formattedType;
                            spaces = 37 - formattedType.length();
                            if (spaces > 0) {
                                for (int i = 0; i < spaces; i++) out << " ";
                            }
                            out << "|  |  |" << std::endl;
                        }
                    }
                }
                
                out << indentStr << "|    |     ==================================================================  |  |" << std::endl;
            }

            out << indentStr << "|    ==========================================================================|  |" << std::endl;
        }
    }

    // Print free functions at global level
    if (table->getScopeName() == "global") {
        // Collect global functions including overloads
        std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> globalFunctions;
        auto allGlobalFunctions = table->getAllFunctions();
        for (const auto& [sig, symbol] : allGlobalFunctions) {
            globalFunctions.push_back({sig.name, symbol});
        }
        
        // Reverse global functions to match declaration order
        std::reverse(globalFunctions.begin(), globalFunctions.end());

        // Print global functions
        for (const auto& [funcName, funcSymbol] : globalFunctions) {
            if (funcSymbol->getKind() == SymbolKind::FUNCTION) {
                // Print function signature
                out << "| function  | " << funcName;
                int spaces = 24 - funcName.length();
                for (int i = 0; i < spaces; i++) out << " ";
                
                // Format params list for display
                std::string paramList = formatFunctionParams(funcSymbol);
                out << "| " << paramList;
                spaces = 42 - paramList.length();
                for (int i = 0; i < spaces; i++) out << " ";
                out << "|" << std::endl;
                
                // Immediately print this function's parameter table
                out << "|     ==========================================================================  |" << std::endl;
                out << "|     | table: ::" << funcName;
                spaces = 62 - funcName.length();
                for (int i = 0; i < spaces; i++) out << " ";
                out << "|  |" << std::endl;
                
                out << "|     ==========================================================================  |" << std::endl;
                
                
                // Print this function's parameters
                const auto& paramInfo = funcSymbol->getParamInfo();
                for (const auto& param : paramInfo) {
                    out << "|     | param     | " << param.name;
                    spaces = 12 - param.name.length();
                    if (spaces > 0) {
                        for (int i = 0; i < spaces; i++) out << " ";
                    }
                    out << "| " << param.type;
                    spaces = 45 - param.type.length();
                    if (spaces > 0) {
                        for (int i = 0; i < spaces; i++) out << " ";
                    }
                    out << "|  |" << std::endl;
                }
                
                // If this function has local variables, print them too
                // Find the correct function subtable by checking parameter lists
                std::string uniqueKey = funcName;
                const auto& paramTypes = funcSymbol->getParams();
                if (!paramTypes.empty()) {
                    uniqueKey += "_" + formatParamTypesForTableName(paramTypes);
                }

                auto functionTable = table->getNestedTable(uniqueKey);
                if (functionTable) {
                    // Print parameters first (existing code)
            
                    // Collect and reverse local variables to match declaration order
                    std::vector<std::pair<std::string, std::shared_ptr<Symbol>>> localVars;
                    for (const auto& [name, symbol] : functionTable->getSymbols()) {
                        if (symbol->getKind() == SymbolKind::VARIABLE) {
                            localVars.push_back({name, symbol});
                        }
                    }
                    std::reverse(localVars.begin(), localVars.end());

                    // Print local variables
                    for (const auto& [localName, localSymbol] : localVars) {
                        out << "|     | local     | " << localName;
                        spaces = 12 - localName.length();
                        if (spaces > 0) {
                            for (int i = 0; i < spaces; i++) out << " ";
                        }
                        std::string formattedType = formatTypeWithDimensions(localSymbol);
                        out << "| " << formattedType;
                        spaces = 45 - formattedType.length();
                        if (spaces > 0) {
                            for (int i = 0; i < spaces; i++) out << " ";
                        }
                        out << "|  |" << std::endl;
                    }
                }
                
                out << "|     ==========================================================================  |" << std::endl;
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
    std::string result = symbol->getType();
    
    // If the type already has array notations, return as is
    if (result.find('[') != std::string::npos) {
        return result;
    }
    
    // Add array dimensions
    const auto& dims = symbol->getArrayDimensions();
    // Reverse dimensions so they appear in the correct order (innermost to outermost)
    std::vector<int> reversedDims = dims;
    std::reverse(reversedDims.begin(), reversedDims.end());
        
    for (int dim : reversedDims) {
        if (dim == -1) {
            // Special code for dynamic arrays
            result += "[]";
        } else {
            result += "[" + std::to_string(dim) + "]";
        }
    }
    
    return result;
}

// Add this helper method:
std::string SymbolTableVisitor::formatFunctionParams(std::shared_ptr<Symbol> funcSymbol) {
    std::string result = "(";
    const auto& params = funcSymbol->getParams();
    for (size_t i = 0; i < params.size(); i++) {
        if (i > 0) result += ", ";
        result += params[i];
    }
    result += "):" + funcSymbol->getType();
    return result;
}

// Add this helper method:
std::string SymbolTableVisitor::formattedParamListForTableName(const std::vector<std::string>& params) {
    std::stringstream ss;
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) ss << "_";
        // Remove special characters and replace spaces with underscores
        std::string paramType = params[i];
        paramType.erase(std::remove_if(paramType.begin(), paramType.end(), 
                      [](char c) { return c == ',' || c == ':' || c == '(' || c == ')' || c == ' '; }), 
                    paramType.end());
        ss << paramType;
    }
    return ss.str();
}

void SymbolTableVisitor::createFunctionSubtable(std::shared_ptr<SymbolTable> parentTable, 
                                               const std::string& prefix,
                                               const std::string& funcName,
                                               std::shared_ptr<Symbol> funcSymbol) {
    // Create display name for function table 
    std::string tableName = (prefix.empty() ? "" : prefix + "::") + funcName;
    
    // Create a unique key for storing in the nested tables map
    std::string uniqueKey = funcName;
    const auto& paramTypes = funcSymbol->getParams();
    if (!paramTypes.empty()) {
        uniqueKey += "_" + formatParamTypesForTableName(paramTypes);
    }
    
    // Create new subtable with the display name
    auto functionTable = std::make_shared<SymbolTable>(tableName, parentTable.get());
    
    // Add parameters to the function table
    const auto& paramInfo = funcSymbol->getParamInfo();
    for (const auto& param : paramInfo) {
        auto paramSymbol = std::make_shared<Symbol>(param.name, param.type, SymbolKind::PARAMETER);
        functionTable->addSymbol(paramSymbol);
    }
    
    // Store the function symbol with this subtable for lookup
    functionTable->setFunctionSymbol(funcSymbol);
    
    // Use the unique key when adding to nestedTables
    parentTable->addNestedTable(uniqueKey, functionTable);
    
    
}

// Helper function to format parameter types for table name
std::string SymbolTableVisitor::formatParamTypesForTableName(const std::vector<std::string>& paramTypes) {
    std::string result;
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        if (i > 0) result += "_";
        // Replace special characters to make a valid table name
        std::string cleaned = paramTypes[i];
        result += cleaned;
    }
    return result;
}


void SymbolTableVisitor::visitIndexList(ASTNode* node) {
    // Process the list
    ASTNode* child = node->getLeftMostChild();
    if (child) {
        child->accept(this);
    }
}

void SymbolTableVisitor::visitDimList(ASTNode* node) {
    // Process each dimension node in the list
    ASTNode* dimNode = node->getLeftMostChild();
    while (dimNode) {
        // Process dimension node - this will add to currentArrayDimensions
        dimNode->accept(this);
        dimNode = dimNode->getRightSibling();
    }
}