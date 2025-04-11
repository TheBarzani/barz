#include "MemSizeVisitor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>

// Since we need to extend Symbol and SymbolTable with additional properties,
// we'll use custom metadata fields to store size and offset information

MemSizeVisitor::MemSizeVisitor(std::shared_ptr<SymbolTable> symbolTable)
    : tempVarCounter(1) {
    
    // Create a deep copy of the symbol table
    this->symbolTable = std::make_shared<SymbolTable>(*symbolTable);
    
    // Set the current table to point to our copy of the symbol table
    currentTable = this->symbolTable;
    
    // Remove all local variables from all function scopes to rebuild them in correct order
    removeLocalVariables(this->symbolTable);
}

MemSizeVisitor::~MemSizeVisitor() {
    // Cleanup if needed
}

void MemSizeVisitor::processAST(ASTNode* root) {
    // Visit the AST to add temporary variables and track expressions
    if (root) {
        root->accept(this);
    }
}

void MemSizeVisitor::calculateMemorySizes() {
    // Start calculating sizes and offsets for the global table first
    calculateTableOffsets(symbolTable);
}

// Helper methods for extending Symbol with size and offset information
void MemSizeVisitor::setSymbolSize(std::shared_ptr<Symbol> symbol, int size) {
    // Store size in Symbol's metadata
    symbol->setMetadata("size", std::to_string(size));
}

void MemSizeVisitor::setSymbolOffset(std::shared_ptr<Symbol> symbol, int offset) {
    // Store offset in Symbol's metadata
    symbol->setMetadata("offset", std::to_string(offset));
}

void MemSizeVisitor::setSymbolTempVarKind(std::shared_ptr<Symbol> symbol, const std::string& kind) {
    // Store temp var kind in Symbol's metadata
    symbol->setMetadata("tempvar_kind", kind);
}

int MemSizeVisitor::getSymbolSize(std::shared_ptr<Symbol> symbol) {
    // Get size from Symbol's metadata
    std::string sizeStr = symbol->getMetadata("size");
    if (!sizeStr.empty()) {
        try {
            return std::stoi(sizeStr);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
    // Default to type size if not explicitly set
    return getTypeSize(symbol->getType());
}

int MemSizeVisitor::getSymbolOffset(std::shared_ptr<Symbol> symbol) {
    // Get offset from Symbol's metadata
    std::string offsetStr = symbol->getMetadata("offset");
    if (!offsetStr.empty()) {
        try {
            return std::stoi(offsetStr);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
    return 0; // Default
}

std::string MemSizeVisitor::getSymbolTempVarKind(std::shared_ptr<Symbol> symbol) {
    // Get temp var kind from Symbol's metadata
    std::string kind = symbol->getMetadata("tempvar_kind");
    if (kind.empty()) {
        // Default handling:
        // 1. Regular variables (x, y, z) should be "var"
        // 2. Temporary variables (t1, t2...) should be properly classified
        if (symbol->getKind() == SymbolKind::PARAMETER || 
            (symbol->getKind() == SymbolKind::VARIABLE && symbol->getName().find("t") != 0)) {
            return "var";
        } else {
            // For temporary variables without explicit kind, default to "tempvar"
            return "tempvar";
        }
    }
    return kind;
}

// Helper methods for extending SymbolTable with scope offset
void MemSizeVisitor::setTableScopeOffset(std::shared_ptr<SymbolTable> table, int offset) {
    // Store scope offset in SymbolTable's metadata
    table->setMetadata("scope_offset", std::to_string(offset));
}

int MemSizeVisitor::getTableScopeOffset(std::shared_ptr<SymbolTable> table) {
    // Get scope offset from SymbolTable's metadata
    std::string offsetStr = table->getMetadata("scope_offset");
    if (!offsetStr.empty()) {
        try {
            return std::stoi(offsetStr);
        } catch (const std::exception& e) {
            // Handle error
        }
    }
    return 0; // Default
}

int MemSizeVisitor::getTypeSize(const std::string& type) {
    // Basic types
    if (type == "int") return TypeSize::INT_SIZE;
    if (type == "float") return TypeSize::FLOAT_SIZE;
    if (type == "void") return TypeSize::VOID_SIZE;
    
    // Check for arrays
    size_t bracketPos = type.find('[');
    if (bracketPos != std::string::npos) {
        // Extract the dimension and base type
        std::string baseType = type.substr(0, bracketPos);
        
        // Extract dimension size between [ and ]
        size_t closeBracketPos = type.find(']', bracketPos);
        if (closeBracketPos == std::string::npos) {
            return TypeSize::POINTER_SIZE; // Malformed array type
        }
        
        std::string dimStr = type.substr(bracketPos + 1, closeBracketPos - bracketPos - 1);
        int dimension = 1; // Default to 1 if no size specified
        
        if (!dimStr.empty()) {
            try {
                dimension = std::stoi(dimStr);
            } catch (const std::exception&) {
                // If not a number, default to 1
                dimension = 1;
            }
        }
        
        // Calculate base element size (which could be another array)
        int elementSize = getTypeSize(baseType);
        
        // Total size is element size multiplied by dimension
        return elementSize * dimension;
    }
    
    // Check for user-defined classes
    auto classTable = symbolTable->getNestedTable(type);
    if (classTable) {
        // Calculate class size as sum of member variable sizes
        int totalSize = 0;
        for (const auto& symbolPair : classTable->getSymbols()) {
            auto symbol = symbolPair.second;
            if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
                int memberSize = 0;
                
                // Check if the member is an array
                if (symbol->isArray()) {
                    // For arrays in classes, calculate size based on type and dimensions
                    std::string memberType = symbol->getType();
                    memberSize = getTypeSize(memberType);
                    
                    // Multiply by each dimension
                    for (int dim : symbol->getArrayDimensions()) {
                        memberSize *= dim;
                    }
                } else {
                    // Normal member, just get its type size
                    memberSize = getTypeSize(symbol->getType());
                }
                
                totalSize += memberSize;
            }
        }
        return totalSize > 0 ? totalSize : TypeSize::POINTER_SIZE;
    }
    
    // Default size for unknown types
    return TypeSize::POINTER_SIZE;
}

void MemSizeVisitor::calculateTableOffsets(std::shared_ptr<SymbolTable> table) {
    if (!table) return;
    
    // Determine if this is a function table using multiple methods
    bool isFunctionTable = false;
    auto funcSymbol = table->getFunctionSymbol();
    
    // Method 1: Check function symbol
    if (funcSymbol && funcSymbol->getKind() == SymbolKind::FUNCTION) {
        isFunctionTable = true;
    }
    // Method 2: Check table name pattern
    else if (table->getScopeName().find("::") == 0 ||
             table->getScopeName().find("_int") != std::string::npos ||
             table->getScopeName().find("_float") != std::string::npos) {
        isFunctionTable = true;
    }
    // Method 3: Check if table contains parameters
    else {
        for (const auto& symbolPair : table->getSymbols()) {
            if (symbolPair.second && symbolPair.second->getKind() == SymbolKind::PARAMETER) {
                isFunctionTable = true;
                break;
            }
        }
    }
    
    int initialOffset = 0;
    
    // For function tables, reserve space for return value and jump register
    if (isFunctionTable) {
        std::string returnType = "void"; // Default
        
        // Try to get return type from function symbol
        if (funcSymbol) {
            returnType = funcSymbol->getType();
        } else {
            // Fallback: Check if we can find a return type in metadata
            std::string returnTypeMetadata = table->getMetadata("return_type");
            if (!returnTypeMetadata.empty()) {
                returnType = returnTypeMetadata;
            }
        }
        
        if (returnType != "void") {
            int returnSize = getTypeSize(returnType);
            // Align return value properly
            int alignment = (returnSize >= 8) ? 8 : 4;
            initialOffset = ((initialOffset - returnSize) / alignment) * alignment;
        }
        
        // Space for jump register (return address) always 4 bytes
        initialOffset -= 4;
        
        // Store this information in table metadata since funcSymbol might be nullptr
        table->setMetadata("return_offset", std::to_string(initialOffset + 4));
        table->setMetadata("link_register_offset", std::to_string(initialOffset));
        
        // If we have a function symbol, also store it there
        if (funcSymbol) {
            funcSymbol->setMetadata("return_offset", std::to_string(initialOffset + 4));
            funcSymbol->setMetadata("link_register_offset", std::to_string(initialOffset));
        }
    }
    
    // Start parameters after the return value and jump register space
    int paramOffset = initialOffset;
    
    // First process parameters with proper sequential offsets
    std::vector<std::shared_ptr<Symbol>> parameters;
    for (const auto& symbolName : table->getSymbolInsertionOrder()) {
        auto symbol = table->lookupSymbol(symbolName, true);
        if (symbol && symbol->getKind() == SymbolKind::PARAMETER) {
            parameters.push_back(symbol);
        }
    }
    
    // Parameters start at the initial offset
    for (size_t i = 0; i < parameters.size(); i++) {
        auto param = parameters[i];
        
        // Calculate size based on type and array dimensions
        int size = getTypeSize(param->getType());
        if (param->isArray()) {
            // For arrays, multiply base size by all dimensions
            for (int dim : param->getArrayDimensions()) {
                size *= dim;
            }
        }
        
        setSymbolSize(param, size);
        
        // Proper alignment for parameters
        int alignment = (size >= 8 || param->isArray()) ? 8 : 4;
        paramOffset = ((paramOffset - size) / alignment) * alignment;
        setSymbolOffset(param, paramOffset);
    }
    
    // Process non-parameters in exact insertion order
    int currentOffset = paramOffset;
    if (currentOffset == initialOffset && parameters.empty()) {
        // If we have no parameters, start local vars after initial offset
        currentOffset = initialOffset;
    }
    
    for (const auto& symbolName : table->getSymbolInsertionOrder()) {
        auto symbol = table->lookupSymbol(symbolName, true);
        if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
            // Calculate size based on type and array dimensions
            int size = getTypeSize(symbol->getType());
            if (symbol->isArray()) {
                // For arrays, multiply base size by all dimensions
                for (int dim : symbol->getArrayDimensions()) {
                    size *= dim;
                }
            }
            
            setSymbolSize(symbol, size);
            
            // Calculate offset with proper alignment
            int alignment = (size >= 8 || symbol->isArray()) ? 8 : 4;
            currentOffset = ((currentOffset - size) / alignment) * alignment;
            setSymbolOffset(symbol, currentOffset);
        }
    }
    
    // Save the total scope offset
    setTableScopeOffset(table, currentOffset);
    
    // Process nested tables
    for (const auto& [name, nestedTable] : table->getNestedTables()) {
        calculateTableOffsets(nestedTable);
    }
}

std::string MemSizeVisitor::createTempVar(const std::string& type, const std::string& kind, ASTNode* node) {
    // Generate unique temp var name
    std::string tempName = "t" + std::to_string(tempVarCounter++);
    
    // Create a symbol for the temp var
    auto tempSymbol = std::make_shared<Symbol>(tempName, type, SymbolKind::VARIABLE);
    setSymbolTempVarKind(tempSymbol, kind);
    
    // Add it to the current table
    if (currentTable) {
        currentTable->addSymbol(tempSymbol);
        
        // Calculate size and offset
        int size = getTypeSize(type);
        setSymbolSize(tempSymbol, size);
        calculateTableOffsets(currentTable);
        // Get current scope offset
        int currentOffset = getTableScopeOffset(currentTable);
        // Calculate offset with proper alignment
        setSymbolOffset(tempSymbol, currentOffset);
        
        // Update table scope offset
        setTableScopeOffset(currentTable, currentOffset);
        
        // Attach metadata to the AST node if provided
        if (node) {
            node->setMetadata("moonVarName", tempName);
            node->setMetadata("offset", std::to_string(currentOffset));
            node->setMetadata("size", std::to_string(size));
            node->setMetadata("type", type);
        }
    }
    
    return tempName;
}

void MemSizeVisitor::outputSymbolTable(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        return;
    }
    
    writeTableToFile(outFile, symbolTable, 0);
    
    outFile.close();
}

// Fixed writeTableToFile to maintain insertion order
void MemSizeVisitor::writeTableToFile(std::ofstream& out, std::shared_ptr<SymbolTable> table, int indent) {
    // Table header code unchanged
    std::string indentStr(indent, ' ');
    int scopeOffset = getTableScopeOffset(table);
    
    out << indentStr << "========================================================" << std::endl;
    out << indentStr << "| table: " << std::left << std::setw(22) << table->getScopeName() 
        << "| scope offset: " << std::setw(7) << scopeOffset << " |" << std::endl;
    out << indentStr << "========================================================" << std::endl;
    
    // Collect symbols in insertion order
    std::vector<std::shared_ptr<Symbol>> allSymbols;
    
    // Parameters first
    for (const auto& symbolName : table->getSymbolInsertionOrder()) {
        auto symbol = table->lookupSymbol(symbolName, true);
        if (symbol && symbol->getKind() == SymbolKind::PARAMETER) {
            allSymbols.push_back(symbol);
        }
    }
    
    // Then variables and temps in insertion order
    for (const auto& symbolName : table->getSymbolInsertionOrder()) {
        auto symbol = table->lookupSymbol(symbolName, true);
        if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
            allSymbols.push_back(symbol);
        }
    }
    
    // Print symbols in insertion order
    for (const auto& symbol : allSymbols) {
        std::string kind = getSymbolTempVarKind(symbol);
        int size = getSymbolSize(symbol);
        int offset = getSymbolOffset(symbol);
        
        out << indentStr << "| " 
            << std::left << std::setw(10) << kind << "| " 
            << std::left << std::setw(13) << symbol->getName() << "| " 
            << std::left << std::setw(10) << symbol->getType() << "| " 
            << std::left << std::setw(6) << size << "| " 
            << std::left << std::setw(6) << offset << "|" << std::endl;
    }
    
    // Rest of method unchanged
    // Collect nested tables in a vector to process them
    std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> nestedTables;
    for (const auto& tablePair : table->getNestedTables()) {
        nestedTables.push_back(tablePair);
    }
    
    // Process nested tables in reversed order
    for (const auto& [name, nestedTable] : nestedTables) {
        // Print nested table with increased indentation
        writeTableToFile(out, nestedTable, indent + 4);
    }
    
    // Close table
    out << indentStr << "========================================================" << std::endl;
}

// Implementation of key visitor methods that need to create temp variables

void MemSizeVisitor::visitProgram(ASTNode* node) {
    // Process all children of the program node
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void MemSizeVisitor::visitFunction(ASTNode* node) {
    // Get function signature
    ASTNode* signatureNode = node->getLeftMostChild();
    if (!signatureNode) return;
    
    // Get function ID
    ASTNode* funcIdNode = signatureNode->getLeftMostChild();
    if (!funcIdNode) return;
    
    // Reset temp variable counter for each function
    // tempVarCounter = 1;
    
    std::string funcName = funcIdNode->getNodeValue();
    currentFunctionName = funcName;
    
    // Get param types to build complete function key
    std::vector<std::string> paramTypes;
    ASTNode* paramListNode = funcIdNode->getRightSibling();
    if (paramListNode && paramListNode->getNodeEnum() == NodeType::PARAM_LIST) {
        ASTNode* paramNode = paramListNode->getLeftMostChild();
        while (paramNode) {
            ASTNode* typeNode = paramNode->getLeftMostChild()->getRightSibling();
            if (typeNode) {
                paramTypes.push_back(typeNode->getNodeValue());
            }
            paramNode = paramNode->getRightSibling();
        }
    }
    
    // Check if this is a class method based on parent nodes
    bool isClassMethod = false;
    std::string className = "";
    
    // Check parent structure to determine if it's a member function
    ASTNode* parentNode = node->getParent();
    if (parentNode && parentNode->getNodeEnum() == NodeType::IMPLEMENTATION_FUNCTION_LIST) {
        // Part of implementation function list - it's a class method
        ASTNode* grandParent = parentNode->getParent();
        if (grandParent && grandParent->getNodeEnum() == NodeType::IMPLEMENTATION) {
            // Get class name from implementation ID
            ASTNode* implIdNode = grandParent->getLeftMostChild();
            if (implIdNode && implIdNode->getNodeEnum() == NodeType::IMPLEMENTATION_ID) {
                className = implIdNode->getNodeValue();
                isClassMethod = true;
            }
        }
    } else if (parentNode && parentNode->getNodeEnum() == NodeType::MEMBER) {
        // Direct member of class - it's a class method declaration
        isClassMethod = true;
        
        // Find containing class by traversing up
        ASTNode* memberListNode = parentNode->getParent();
        if (memberListNode && memberListNode->getNodeEnum() == NodeType::MEMBER_LIST) {
            ASTNode* classNode = memberListNode->getParent();
            if (classNode && classNode->getNodeEnum() == NodeType::CLASS) {
                ASTNode* classIdNode = classNode->getLeftMostChild();
                if (classIdNode && classIdNode->getNodeEnum() == NodeType::CLASS_ID) {
                    className = classIdNode->getNodeValue();
                }
            }
        }
    }
    
    // Build table key based on whether it's a class method
    std::string tableKey = funcName;
    
    // Add parameter types if any
    if (!paramTypes.empty()) {
        tableKey += "_";
        for (size_t i = 0; i < paramTypes.size(); i++) {
            if (i > 0) tableKey += "_";
            tableKey += paramTypes[i];
        }
    }
    
    // Find function/method table
    auto prevTable = currentTable;
    auto funcTable = currentTable->getNestedTable(tableKey);
    
    if (funcTable) {
        currentTable = funcTable;
    }
    
    // Process function body
    ASTNode* bodyNode = signatureNode->getRightSibling();
    if (bodyNode) {
        bodyNode->accept(this);
    }
    
    // Restore previous table
    currentTable = prevTable;
}

void MemSizeVisitor::visitAssignment(ASTNode* node) {
    // Get left and right parts of the assignment
    ASTNode* leftNode = node->getLeftMostChild();
    ASTNode* rightNode = leftNode ? leftNode->getRightSibling() : nullptr;
    
    // Check if right-hand side is a direct literal (INT or FLOAT)
    if (rightNode && (rightNode->getNodeEnum() == NodeType::INT || 
                      rightNode->getNodeEnum() == NodeType::FLOAT)) {
        // Create a temporary variable for the literal
        rightNode->accept(this);
    }
    // Check if right-hand side is an operator (ADD_OP or MULT_OP)
    else if (rightNode && (rightNode->getNodeEnum() == NodeType::ADD_OP || 
                          rightNode->getNodeEnum() == NodeType::MULT_OP)) {
        // Process the operands first
        rightNode->accept(this);
    }
    // All other cases
    else if (rightNode) {
        rightNode->accept(this);
    }
    
    // Process left-hand side
    if (leftNode) {
        leftNode->accept(this);
    }
}

void MemSizeVisitor::visitFunctionCall(ASTNode* node) {
    // First check if this is a member function call (e.g., obj.func())
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (!funcIdNode) return;
    
    std::string funcName = funcIdNode->getNodeValue();
    bool isMemberFunction = false;
    std::string className = "";
    
    // Check if this is a dot identifier (member access)
    if (funcIdNode->getNodeEnum() == NodeType::DOT_IDENTIFIER) {
        // Get the class name from the left child
        ASTNode* objNode = funcIdNode->getParent()->getParent()->getLeftMostChild();
        if (objNode) {
            // Find the type of the object
            std::string objName = objNode->getNodeValue();
            auto objSymbol = currentTable->lookupSymbol(objName);
            if (objSymbol) {
                className = objSymbol->getType();
                isMemberFunction = true;
            }
        }
    }
    
    // Save current table for restoration later
    auto prevTable = currentTable;
    
    // Get parameter types to construct the function key
    std::vector<std::string> paramTypes;
    ASTNode* argListNode = funcIdNode->getRightSibling();
    if (argListNode) {
        ASTNode* argNode = argListNode->getLeftMostChild();
        while (argNode) {
            // Determine the actual type of the argument
            std::string argType = determineNodeType(argNode);
            paramTypes.push_back(argType);
            
            // Process the argument node
            argNode->accept(this);
            
            // Move to next argument
            argNode = argNode->getRightSibling();
        }
    }
    
    // Construct function key with parameter types
    std::string funcKey = funcName;
    if (!paramTypes.empty()) {
        funcKey += "_";
        for (size_t i = 0; i < paramTypes.size(); i++) {
            if (i > 0) funcKey += "_";
            funcKey += paramTypes[i];
        }
    }
    
    // If this is a member function, switch to appropriate class scope
    if (isMemberFunction && !className.empty()) {
        // Find the class table
        auto classTable = symbolTable->getNestedTable(className);
        if (classTable) {
            // Try different format options for the method table key
            std::vector<std::string> possibleKeys = {
                funcKey,                         // With parameter types: build_float_float
                className + "::" + funcName,     // Standard format: CLASS::method
                funcName + "_" + className,      // Alternate format: method_CLASS
                funcName                         // Just the method name within class
            };
            
            // Try each key format
            for (const auto& key : possibleKeys) {
                auto methodTable = classTable->getNestedTable(key);
                if (methodTable) {
                    currentTable = methodTable;
                    break;
                }
            }
        }
    }
    
    // Generate return value temp var in the appropriate scope
    auto funcSymbols = currentTable->lookupFunctions(funcName, false); // Include parent scopes
    if (!funcSymbols.empty()) {
        currentTable = prevTable;
        std::string returnType = funcSymbols[0]->getType();
        if (returnType != "void") {
            createTempVar(returnType, "retval", node);
        }
    }
    
    // Restore original scope
    currentTable = prevTable;
}

// Add this helper method to determine a node's type
std::string MemSizeVisitor::determineNodeType(ASTNode* node) {
    if (!node) return "int"; // Default fallback
    
    // Direct type determination for simple nodes
    if (node->getNodeEnum() == NodeType::INT) {
        return "int";
    } else if (node->getNodeEnum() == NodeType::FLOAT) {
        return "float";
    } else if (node->getNodeEnum() == NodeType::IDENTIFIER) {
        // Look up the identifier in symbol table
        auto symbol = currentTable->lookupSymbol(node->getNodeValue());
        if (symbol) {
            return symbol->getType();
        }
    } else if (node->getNodeEnum() == NodeType::FUNCTION_CALL) {
        // Get the function name
        ASTNode* funcIdNode = node->getLeftMostChild();
        if (funcIdNode) {
            std::string funcName = funcIdNode->getNodeValue();
            // Look up the function in symbol table
            auto funcSymbols = currentTable->lookupFunctions(funcName, false);
            if (!funcSymbols.empty()) {
                return funcSymbols[0]->getType();
            }
        }
    } else if (node->getNodeEnum() == NodeType::ADD_OP || 
               node->getNodeEnum() == NodeType::MULT_OP) {
        // For operations, determine based on operands
        ASTNode* leftChild = node->getLeftMostChild();
        ASTNode* rightChild = leftChild ? leftChild->getRightSibling() : nullptr;
        
        if (leftChild && rightChild) {
            std::string leftType = determineNodeType(leftChild);
            std::string rightType = determineNodeType(rightChild);
            
            // Promotion rules - float has precedence over int
            if (leftType == "float" || rightType == "float") {
                return "float";
            } else {
                return "int";
            }
        }
    } else if (node->getNodeEnum() == NodeType::REL_OP) {
        // Relational operations always produce boolean results (int in our implementation)
        return "int";
    }
    
    // Default fallback
    return "int";
}

void MemSizeVisitor::visitRelationalExpr(ASTNode* node) {
    // Process left operand
    ASTNode* leftNode = node->getLeftMostChild();
    if (leftNode) {
        leftNode->accept(this);
    }
    
    // Process right operand
    ASTNode* opNode = leftNode ? leftNode->getRightSibling() : nullptr;
    ASTNode* rightNode = opNode ? opNode->getRightSibling() : nullptr;
    if (rightNode) {
        rightNode->accept(this);
    }
    
    // Create temp var for the result
    createTempVar("int", "tempvar", node); // Boolean result represented as int
}

void MemSizeVisitor::visitArithExpr(ASTNode* node) {
    // Process first term
    ASTNode* termNode = node->getLeftMostChild();
    if (termNode) {
        termNode->accept(this);
    }
    
    // Process additional terms with add/subtract operators
    ASTNode* current = termNode ? termNode->getRightSibling() : nullptr;
    while (current) {
        if (current->getNodeEnum() == NodeType::ADD_OP) {
            // Get the next term
            ASTNode* nextTerm = current->getRightSibling();
            if (nextTerm) {
                nextTerm->accept(this);
                
                // Create a temp var for the result
                createTempVar("int", "tempvar", node); // Assuming int for simplicity
            }
            current = nextTerm ? nextTerm->getRightSibling() : nullptr;
        } else {
            current = current->getRightSibling();
        }
    }
}

void MemSizeVisitor::visitTerm(ASTNode* node) {
    // Process first factor
    ASTNode* factorNode = node->getLeftMostChild();
    if (factorNode) {
        factorNode->accept(this);
    }
    
    // Process additional factors with mult/div operators
    ASTNode* current = factorNode ? factorNode->getRightSibling() : nullptr;
    while (current) {
        if (current->getNodeEnum() == NodeType::MULT_OP) {
            // Get the next factor
            ASTNode* nextFactor = current->getRightSibling();
            if (nextFactor) {
                nextFactor->accept(this);
                
                // Create a temp var for the result
                createTempVar("int", "tempvar", node); // Assuming int for simplicity
            }
            current = nextFactor ? nextFactor->getRightSibling() : nullptr;
        } else {
            current = current->getRightSibling();
        }
    }
}

void MemSizeVisitor::visitInt(ASTNode* node) {
    // Only create a temp var if this int is not part of an operation
    // Let ADD_OP and MULT_OP handle their operands
    if (node->getParent()) {
        createTempVar("int", "litval",node);
    }
}

void MemSizeVisitor::visitFloat(ASTNode* node) {
    // Only create a temp var if this int is not part of an operation
    // Let ADD_OP and MULT_OP handle their operands
    if (node->getParent()) {
        createTempVar("float", "litval", node);
    }
}

void MemSizeVisitor::visitAddOp(ASTNode* node) {
    // Visit left operand
    ASTNode* leftOperand = node->getLeftMostChild();
    if (leftOperand) {
        leftOperand->accept(this);
    }
    
    // Visit right operand
    ASTNode* rightOperand = leftOperand ? leftOperand->getRightSibling() : nullptr;
    if (rightOperand) {
        rightOperand->accept(this);
    }
    
    // Create a temporary variable for the result and attach to node
    createTempVar("int", "tempvar", node);
}

void MemSizeVisitor::visitMultOp(ASTNode* node) {
    // Visit left operand
    ASTNode* leftOperand = node->getLeftMostChild();
    if (leftOperand) {
        leftOperand->accept(this);
    }
    
    // Visit right operand
    ASTNode* rightOperand = leftOperand ? leftOperand->getRightSibling() : nullptr;
    if (rightOperand) {
        rightOperand->accept(this);
    }
    
    // Create a temporary variable for the result
    createTempVar("int", "tempvar", node);  // Adjust type as needed
}

void MemSizeVisitor::visitImplementation(ASTNode* node) {
    // This handles a class implementation section
    // Get class ID node
    ASTNode* implIdNode = node->getLeftMostChild();
    if (!implIdNode || implIdNode->getNodeEnum() != NodeType::IMPLEMENTATION_ID) return;
    
    // Get class name
    std::string className = implIdNode->getNodeValue();
    
    // Find class table
    auto classTable = symbolTable->getNestedTable(className);
    if (!classTable) return;
    
    // Save current table and switch to class table for method implementations
    auto prevTable = currentTable;
    currentTable = classTable;
    
    // Process implementation function list (methods)
    ASTNode* implFuncListNode = implIdNode->getRightSibling();
    if (implFuncListNode) {
        implFuncListNode->accept(this);
    }
    
    // Restore previous table
    currentTable = prevTable;
}

void MemSizeVisitor::visitImplementationFunctionList(ASTNode* node) {
    // Process each method implementation
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void MemSizeVisitor::visitLocalVariable(ASTNode* node) {
    // Get variable ID and type nodes
    ASTNode* varIdNode = node->getLeftMostChild();
    ASTNode* typeNode = varIdNode ? varIdNode->getRightSibling() : nullptr;
    
    if (varIdNode && typeNode) {
        // Get the variable name
        std::string varName = varIdNode->getNodeValue();
        
        // Look it up in the current table - it should already exist
        auto varSymbol = currentTable->lookupSymbol(varName, true); // localOnly=true
        
        if (!varSymbol) {
            // If it doesn't exist, create it
            std::string typeName = typeNode->getNodeValue();
            auto newSymbol = std::make_shared<Symbol>(varName, typeName, SymbolKind::VARIABLE);
            currentTable->addSymbol(newSymbol);
        } else {
            // Important: Re-add it to the symbol table to update its position in insertion order
            // This will place it after any temp vars created before this local variable
            currentTable->removeSymbol(varName);
            currentTable->addSymbol(varSymbol);
        }
        if(typeNode->getNodeEnum() == NodeType::ARRAY_TYPE){
            typeNode->accept(this); // Visit array type node to calculate size
        }
    }
}

void MemSizeVisitor::visitRelOp(ASTNode* node) {
    // Visit left operand
    ASTNode* leftOperand = node->getLeftMostChild();
    if (leftOperand) {
        leftOperand->accept(this);
    }
    
    // Visit right operand
    ASTNode* rightOperand = leftOperand ? leftOperand->getRightSibling() : nullptr;
    if (rightOperand) {
        rightOperand->accept(this);
    }
    
    // Create a temporary variable to store the comparison result (boolean as int)
    createTempVar("int", "tempvar", node);
}

// void MemSizeVisitor::visitFunctionBody(ASTNode* node) {
//     // Track all symbols that need to be properly ordered
//     std::set<std::string> localVars;
    
//     // First pass - collect all existing local variables
//     for (const auto& symbolPair : currentTable->getSymbols()) {
//         auto symbol = symbolPair.second;
//         if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
//             localVars.insert(symbol->getName());
//         }
//     }
    
//     // Process all statements in order
//     ASTNode* child = node->getLeftMostChild();
//     while (child) {
//         // Special handling for local variable declarations
//         if (child->getNodeEnum() == NodeType::LOCAL_VARIABLE) {
//             ASTNode* varIdNode = child->getLeftMostChild();
//             if (varIdNode) {
//                 std::string varName = varIdNode->getNodeValue();
                
//                 // If it exists, remove it so we can reorder
//                 auto varSymbol = currentTable->lookupSymbol(varName, true);
//                 if (varSymbol) {
//                     currentTable->removeSymbol(varName);
//                 }
                
//                 // Process the declaration normally
//                 child->accept(this);
                
//                 // Mark as processed
//                 localVars.insert(varName);
//             }
//         } else {
//             // Process other statements normally
//             child->accept(this);
//         }
        
//         child = child->getRightSibling();
//     }
    
//     // Force recalculation of all offsets after all statements are processed
//     calculateTableOffsets(currentTable);
// }

// For simplicity, I'm just implementing the main visitor methods
// All other visitor methods would just traverse the AST without special handling
// Implement the rest of the visitor methods as needed

// Default implementation for other visitor methods that just traverse the AST
#define DEFAULT_VISITOR_METHOD(method) \
    void MemSizeVisitor::visit##method(ASTNode* node) { \
        ASTNode* child = node->getLeftMostChild(); \
        while (child) { \
            child->accept(this); \
            child = child->getRightSibling(); \
        } \
    }

DEFAULT_VISITOR_METHOD(ClassList)
DEFAULT_VISITOR_METHOD(Class)
DEFAULT_VISITOR_METHOD(ClassId)
DEFAULT_VISITOR_METHOD(InheritanceList)
DEFAULT_VISITOR_METHOD(InheritanceId)
DEFAULT_VISITOR_METHOD(Visibility)
DEFAULT_VISITOR_METHOD(MemberList)
DEFAULT_VISITOR_METHOD(FunctionBody)
DEFAULT_VISITOR_METHOD(Empty)
DEFAULT_VISITOR_METHOD(FunctionList)
DEFAULT_VISITOR_METHOD(ImplementationList)
DEFAULT_VISITOR_METHOD(Member)
DEFAULT_VISITOR_METHOD(Variable)
DEFAULT_VISITOR_METHOD(VariableId)
DEFAULT_VISITOR_METHOD(FunctionId)
DEFAULT_VISITOR_METHOD(FunctionSignature)
DEFAULT_VISITOR_METHOD(ConstructorSignature)
DEFAULT_VISITOR_METHOD(ParamList)
DEFAULT_VISITOR_METHOD(Param)
DEFAULT_VISITOR_METHOD(ParamId)
DEFAULT_VISITOR_METHOD(Type)
DEFAULT_VISITOR_METHOD(ArrayType)
DEFAULT_VISITOR_METHOD(ArrayDimension)
DEFAULT_VISITOR_METHOD(Block)
DEFAULT_VISITOR_METHOD(StatementsList)
DEFAULT_VISITOR_METHOD(IfStatement)
DEFAULT_VISITOR_METHOD(WhileStatement)
DEFAULT_VISITOR_METHOD(Condition)
DEFAULT_VISITOR_METHOD(SingleStatement)
DEFAULT_VISITOR_METHOD(ExpressionStatement)
DEFAULT_VISITOR_METHOD(ReadStatement)
DEFAULT_VISITOR_METHOD(WriteStatement)
DEFAULT_VISITOR_METHOD(ReturnStatement)
DEFAULT_VISITOR_METHOD(FunctionDeclaration)
DEFAULT_VISITOR_METHOD(Identifier)
DEFAULT_VISITOR_METHOD(SelfIdentifier)
DEFAULT_VISITOR_METHOD(DotIdentifier)
DEFAULT_VISITOR_METHOD(DotAccess)
DEFAULT_VISITOR_METHOD(ArrayAccess)
DEFAULT_VISITOR_METHOD(Expr)
DEFAULT_VISITOR_METHOD(Factor)
DEFAULT_VISITOR_METHOD(AssignOp)
// DEFAULT_VISITOR_METHOD(RelOp)
DEFAULT_VISITOR_METHOD(Attribute)
DEFAULT_VISITOR_METHOD(ImplementationId)

// Add this new helper method to remove local variables
void MemSizeVisitor::removeLocalVariables(std::shared_ptr<SymbolTable> table) {
    if (!table) return;
    
    // Determine if this is a function table using multiple methods
    bool isFunctionTable = false;
    
    // Method 1: Check function symbol
    auto funcSymbol = table->getFunctionSymbol();
    if (funcSymbol && funcSymbol->getKind() == SymbolKind::FUNCTION) {
        isFunctionTable = true;
    }
    // Method 2: Check table name pattern (typically function tables have specific naming)
    else if (table->getScopeName().find("::") == 0 ||          // Global function like "::main"
             table->getScopeName().find("_int") != std::string::npos ||  // Function with int params
             table->getScopeName().find("_float") != std::string::npos) { // Function with float params
        isFunctionTable = true;
    }
    // Method 3: Check if table contains parameters
    else {
        for (const auto& symbolPair : table->getSymbols()) {
            if (symbolPair.second && symbolPair.second->getKind() == SymbolKind::PARAMETER) {
                isFunctionTable = true;
                break;
            }
        }
    }
    
    if (isFunctionTable) {
        // This is a function scope - remove all non-parameter variables
        std::vector<std::string> variablesToRemove;
        
        // First identify all variables to remove
        for (const auto& symbolPair : table->getSymbols()) {
            auto symbol = symbolPair.second;
            if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
                // Don't remove temporary variables that start with 't'
                if (symbol->getName()[0] != 't') {
                    variablesToRemove.push_back(symbol->getName());
                }
            }
        }
        
        // Then remove them
        for (const auto& varName : variablesToRemove) {
            table->removeSymbol(varName);
        }
    }
    
    // Recursively process nested tables
    for (const auto& [name, nestedTable] : table->getNestedTables()) {
        removeLocalVariables(nestedTable);
    }
}