#include "MemSizeVisitor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>
#include <iomanip>
#include <regex>

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
            (symbol->getKind() == SymbolKind::VARIABLE && 
             // Check for compiler-generated temp vars that match pattern "t" followed by digits
             !std::regex_match(symbol->getName(), std::regex("t[0-9]+")))) {
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

// Helper to extract base type from a type string possibly containing dimensions
std::string MemSizeVisitor::getBaseType(const std::string& typeWithDimensions) {
    size_t bracketPos = typeWithDimensions.find('[');
    if (bracketPos != std::string::npos) {
        return typeWithDimensions.substr(0, bracketPos);
    }
    return typeWithDimensions; // No brackets found, return the original string
}

// Updated getTypeSize to primarily handle base types and class names
int MemSizeVisitor::getTypeSize(const std::string& baseType) {
    if (baseType == "int") return TypeSize::INT_SIZE;
    if (baseType == "float") return TypeSize::FLOAT_SIZE;
    if (baseType == "void") return TypeSize::VOID_SIZE;

    // Check for user-defined classes by looking up the name in the global scope
    // Assuming class definitions are always in the global scope's nested tables.
    auto classTable = symbolTable->getNestedTable(baseType);
    if (classTable) {
        // If the size is already calculated and stored, use it
        std::string sizeMeta = classTable->getMetadata("scope_offset");
        if (!sizeMeta.empty()) {
            try {
                return (std::stoi(sizeMeta)*-1);
            } catch (...) { /* ignore error, recalculate */ }
        }
        return TypeSize::POINTER_SIZE; // Or calculate recursively if safe
    }

    // Default size for unknown types (treat as pointers or handle error)
    return TypeSize::POINTER_SIZE;
}

// Helper to check if a table represents a function scope
bool MemSizeVisitor::isFunctionTable(std::shared_ptr<SymbolTable> table) {
    // Method 1: Check for function symbol
    auto funcSymbol = table->getFunctionSymbol();
    if (funcSymbol && funcSymbol->getKind() == SymbolKind::FUNCTION) {
        return true;
    }
    
    // Method 2: Check table name patterns for function tables
    if (table->getScopeName().find("::") == 0 ||          // Global function like "::main"
        table->getScopeName().find("_int") != std::string::npos ||  // Function with int params
        table->getScopeName().find("_float") != std::string::npos) { // Function with float params
        return true;
    }
    
    // Method 3: Check if table contains parameters
    for (const auto& symbolPair : table->getSymbols()) {
        if (symbolPair.second && symbolPair.second->getKind() == SymbolKind::PARAMETER) {
            return true;
        }
    }
    
    return false;
}

// Helper to calculate initial offset for function tables (return value + link register)
int MemSizeVisitor::calculateFunctionInitialOffset(std::shared_ptr<SymbolTable> table) {
    int offset = 0;
    auto funcSymbol = table->getFunctionSymbol();
    
    // Get return type
    std::string returnType = "void";
    if (funcSymbol) {
        returnType = funcSymbol->getType();
    } else {
        std::string returnTypeMetadata = table->getMetadata("return_type");
        if (!returnTypeMetadata.empty()) {
            returnType = returnTypeMetadata;
        }
    }
    
    // Reserve space for return value if not void
    std::string baseReturnType = getBaseType(returnType);
    if (baseReturnType != "void") {
        int returnSize = getTypeSize(baseReturnType);
        offset -= returnSize; // First subtract the size
        
        // Align to appropriate boundary (8 for large types, 4 for others)
        int alignment = (returnSize >= 8) ? 8 : 4;
        offset = (offset / alignment) * alignment; // Integer division rounds toward 0
        // For negative numbers, we need to explicitly round down if not aligned
        if (offset % alignment != 0) {
            offset -= alignment - ((-offset) % alignment);
        }
    }
    
    // Reserve space for link register (always 4 bytes)
    offset -= 4;
    // Link register should be aligned to 4-byte boundary
    offset = (offset / 4) * 4;
    
    // Set metadata with corrected offsets
    int returnOffset = offset; // If void, there's no return value
    table->setMetadata("return_offset", std::to_string(returnOffset));
    table->setMetadata("link_register_offset", std::to_string(offset));
    
    if (funcSymbol) {
        funcSymbol->setMetadata("return_offset", std::to_string(returnOffset));
        funcSymbol->setMetadata("link_register_offset", std::to_string(offset));
    }
    
    return offset;
}

// Main method to calculate offsets for a table and its contents
void MemSizeVisitor::calculateTableOffsets(std::shared_ptr<SymbolTable> table) {
    if (!table) return;
    
    bool isGlobalTable = (table == symbolTable && table->getScopeName() == "global");
    
    // Determine if this table represents a function
    bool isFunction = isFunctionTable(table);
    
    // PASS 1: Calculate sizes for class tables first
    // Process class tables first so their sizes are available when needed
    for (const auto& tableName : table->getNestedTableInsertionOrder()) {
        auto nestedTable = table->getNestedTable(tableName);
        if (!nestedTable) continue;
        
        // Skip global table and function tables - only process class tables
        bool isClassTable = !isFunction && 
                          nestedTable->getScopeName() != "global" && 
                          !isFunctionTable(nestedTable);
        
        if (isClassTable && nestedTable->getMetadata("size").empty()) {
            // Recursively calculate offsets for the class members
            calculateTableOffsets(nestedTable);
            
            // Calculate total class size (from 0 to lowest offset)
            int classTotalSize = -getTableScopeOffset(nestedTable);
            
            // Ensure minimum size for empty classes
            if (classTotalSize <= 0) {
                classTotalSize = TypeSize::POINTER_SIZE;
            }
            
            // Store class size in metadata
            nestedTable->setMetadata("size", std::to_string(classTotalSize));
        }
    }
    
    // Calculate initial offset (0 for classes, function-specific for functions)
    int initialOffset = isFunction ? calculateFunctionInitialOffset(table) : 0;
    int currentOffset = initialOffset;
    
    // PASS 2: Process symbols in this table, calculating offsets
    for (const auto& symbolName : table->getSymbolInsertionOrder()) {
        auto symbol = table->lookupSymbol(symbolName, true);
        if (!symbol) continue;
        
        // Skip function symbols - they don't need stack space allocation
        if (symbol->getKind() == SymbolKind::FUNCTION) {
            continue;
        }
        
        // Calculate symbol size based on type and dimensions
        std::string baseType = getBaseType(symbol->getType());
        int size = getTypeSize(baseType);
        
        // For arrays, multiply size by all dimensions
        if (symbol->isArray()) {
            for (int dim : symbol->getArrayDimensions()) {
                if (dim > 0) {
                    size *= dim;
                } else {
                    size = TypeSize::POINTER_SIZE; // Dynamic array
                    break;
                }
            }
        }
        
        setSymbolSize(symbol, size);
        
        currentOffset = ((currentOffset - size));
        setSymbolOffset(symbol, currentOffset);
    }
    
    // Save the total scope offset
    // For global table, always set to 0
    if (isGlobalTable) {
        setTableScopeOffset(table, 0);  // Global table always has offset 0
    } else {
        setTableScopeOffset(table, currentOffset); // Other tables use calculated offset
    }
    // PASS 3: Process remaining nested function tables
    for (const auto& [name, nestedTable] : table->getNestedTables()) {
        // Skip tables already processed in PASS 1
        if (nestedTable->getMetadata("size").empty()) {
            calculateTableOffsets(nestedTable);
        }
    }
}

// Updated createTempVar to use the refined offset calculation
std::string MemSizeVisitor::createTempVar(const std::string& type, const std::string& kind, ASTNode* node) {
    std::string tempName = "t" + std::to_string(tempVarCounter++);
    auto tempSymbol = std::make_shared<Symbol>(tempName, type, SymbolKind::VARIABLE);
    setSymbolTempVarKind(tempSymbol, kind);

    if (currentTable) {
        currentTable->addSymbol(tempSymbol); // Add to table AND insertion order

        // Calculate size (only base type needed for temps)
        int size = getTypeSize(type);
        setSymbolSize(tempSymbol, size);

        // Get current scope offset *before* adding this temp var
        int currentScopeEndOffset = getTableScopeOffset(currentTable);

        // Calculate offset for this new temp var
        int alignment = (size >= 8) ? 8 : 4;
        int newOffset = ((currentScopeEndOffset - size) / alignment) * alignment;
        setSymbolOffset(tempSymbol, newOffset);

        // Update table's total scope offset
        setTableScopeOffset(currentTable, newOffset);

        // Attach metadata to the AST node if provided
        if (node) {
            node->setMetadata("moonVarName", tempName);
            node->setMetadata("offset", std::to_string(newOffset));
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

// writeTableToFile to maintain insertion order
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
    
    // Process nested tables in proper insertion order
    // ----------- CHANGED CODE STARTS HERE -----------
    
    // Use the table's getNestedTableInsertionOrder() method if available
    // If not available, we'll need a fallback
    std::vector<std::string> nestedTableOrder;
    

    nestedTableOrder = table->getNestedTableInsertionOrder();
    
    // Process nested tables in the determined order
    for (const auto& tableName : nestedTableOrder) {
        auto nestedTable = table->getNestedTable(tableName);
        if (nestedTable) {
            // Print nested table with increased indentation
            writeTableToFile(out, nestedTable, indent + 4);
        }
    }
    
    // ----------- CHANGED CODE ENDS HERE -----------
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

// Updated visitFunction to handle new Param structure indirectly
// Updated visitFunction to properly handle function table lookup with parameter types
void MemSizeVisitor::visitFunction(ASTNode* node) {
    // Get function name and signature
    ASTNode* signatureNode = node->getLeftMostChild();
    if (!signatureNode) return;
    ASTNode* funcIdNode = signatureNode->getLeftMostChild();
    if (!funcIdNode) return;
    std::string funcName = funcIdNode->getNodeValue();
    currentFunctionName = funcName;

    // Check if class method context needs to be established
    bool isClassMethod = false;
    std::string className = "";
    ASTNode* parentNode = node->getParent();
    if (parentNode && parentNode->getNodeEnum() == NodeType::IMPLEMENTATION_FUNCTION_LIST) {
        ASTNode* grandParent = parentNode->getParent();
        if (grandParent && grandParent->getNodeEnum() == NodeType::IMPLEMENTATION) {
            ASTNode* implIdNode = grandParent->getLeftMostChild();
            if (implIdNode && implIdNode->getNodeEnum() == NodeType::IMPLEMENTATION_ID) {
                className = implIdNode->getNodeValue();
                isClassMethod = true;
            }
        }
    } else if (parentNode && parentNode->getNodeEnum() == NodeType::MEMBER) {
        isClassMethod = true;
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

    auto prevTable = currentTable;
    std::shared_ptr<SymbolTable> funcTable = nullptr;
    
    // Extract parameter types from function signature
    std::vector<std::string> paramTypes;
    ASTNode* paramListNode = nullptr;
    
    // Find the parameter list node
    ASTNode* child = signatureNode->getLeftMostChild(); // Skip function ID
    while (child) {
        if (child->getNodeEnum() == NodeType::PARAM_LIST) {
            paramListNode = child;
            break;
        }
        child = child->getRightSibling();
    }
    
    // Process parameter types
    if (paramListNode) {
        ASTNode* paramNode = paramListNode->getLeftMostChild();
        while (paramNode) {
            // Get parameter type
            ASTNode* paramIdNode = paramNode->getLeftMostChild();
            ASTNode* typeNode = paramIdNode ? paramIdNode->getRightSibling() : nullptr;
            
            if (typeNode) {
                std::string paramType = typeNode->getNodeValue();
                
                // Check for dimension list as the rightmost child (array type)
                ASTNode* dimListNode = typeNode->getRightSibling();
                bool isArray = false;
                
                if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
                    // Check if the dimension list has any children
                    if (dimListNode->getLeftMostChild() && dimListNode->getLeftMostChild()->getNodeValue() == "dynamic") {
                        isArray = true;
                        // Mark this as an array type for the function key
                        paramType += "[]";
                    }
                    else if(dimListNode->getLeftMostChild()){
                        isArray = true;
                        // Check if the dimension list has a fixed size
                        ASTNode* dimNode = dimListNode->getLeftMostChild();
                        while (dimNode) {  
                            paramType += "[";
                            paramType += dimNode->getNodeValue();
                            paramType += "]";
                            dimNode = dimNode->getRightSibling();
                        }

                    }
                }
                paramTypes.push_back(paramType);
                // // Convert to simplified type for table key
                // if (paramType == "int" || paramType == "float" || 
                //     paramType == "int[]" || paramType == "float[]") {
                //     paramTypes.push_back(paramType);
                // } else if (isArray) {
                //     // Custom array types
                //     paramTypes.push_back("class[]");
                // } else {
                //     // Custom types are typically treated as "class" in the table key
                //     paramTypes.push_back("class");
                // }
            }
            
            paramNode = paramNode->getRightSibling();
        }
    }
    
    // Format the function name with parameter types
    std::string formattedFuncName = funcName;
    if (!paramTypes.empty()) {
        formattedFuncName += "_" + paramTypes[0];
        for (size_t i = 1; i < paramTypes.size(); ++i) {
            formattedFuncName += "_" + paramTypes[i];
        }
    }
    
    // For class methods, try multiple possible table key formats
    if (isClassMethod && !className.empty()) {
        auto classTable = symbolTable->getNestedTable(className); // Always use global scope to find class
        
        if (classTable) {
            // Try different naming conventions for method tables
            std::vector<std::string> possibleKeys = {
                className + "::" + formattedFuncName,    // Standard format with params: CLASS::method_int_float
                className + "::" + funcName,             // Standard format without params: CLASS::method
                formattedFuncName,                       // Just formatted name: method_int_float
                funcName,                                // Just method name: method
                funcIdNode->getMetadata("table_key")     // Metadata if available
            };
            
            // Try each key format
            for (const auto& key : possibleKeys) {
                if (!key.empty()) {
                    funcTable = classTable->getNestedTable(key);
                    if (funcTable) {
                        // std::cout << "Found method table with key: " << key << std::endl;
                        break;
                    }
                }
            }
            
            if (funcTable) {
                currentTable = funcTable;
                createTempVar("int", "addrvar", funcIdNode); // Create a temp var for the return value
            } else {
                // Could not find method table - error case
                std::cerr << "Could not find symbol table for method " << funcName 
                          << " in class " << className << std::endl;
                std::cerr << "Tried keys with param types: " << formattedFuncName << std::endl;
            }
        }
    } else {
        // For global functions
        std::string tableKey = funcIdNode->getMetadata("table_key");
        
        if (tableKey.empty()) {
            // Try standard formats for global functions
            std::vector<std::string> possibleKeys = {
                "::" + formattedFuncName,    // With parameter types: ::func_int_float
                "::" + funcName,             // Just function name: ::func
                formattedFuncName,           // Formatted name without :: prefix
                funcName                     // Just func name without prefix
            };
            
            for (const auto& key : possibleKeys) {
                funcTable = symbolTable->getNestedTable(key);
                if (funcTable) {
                    std::cout << "Found global function table with key: " << key << std::endl;
                    break;
                }
            }
        } else {
            funcTable = symbolTable->getNestedTable(tableKey);
        }
        
        if (funcTable) currentTable = funcTable;
    }

    if (!funcTable) {
        // Error: Function table not found, cannot process body
        std::cerr << "Could not find table for function: " << formattedFuncName << std::endl;
        currentTable = prevTable; // Restore previous table
        return;
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

// Updated visitLocalVariable to just visit the child Variable node
void MemSizeVisitor::visitLocalVariable(ASTNode* node) {
    // The only child should be a Variable node
    ASTNode* variableNode = node->getLeftMostChild();
    if (variableNode && variableNode->getNodeEnum() == NodeType::VARIABLE) {
        // Process the underlying variable declaration
        // This will add/update the symbol in the currentTable's insertion order
        variableNode->accept(this);
    } else {
        // reportError("LocalVariable node expects a Variable node as its child.", node);
    }
}

void MemSizeVisitor::visitVariable(ASTNode* node) {
    // Child 1: Variable ID
    ASTNode* varIdNode = node->getLeftMostChild();
    if (!varIdNode) return;
    std::string varName = varIdNode->getNodeValue();

    // Child 2: Type
    ASTNode* typeNode = varIdNode->getRightSibling();
    if (!typeNode) return;
    // Visit type to potentially set currentType (though not strictly needed here)
    typeNode->accept(this);
    std::string baseTypeName = typeNode->getNodeValue(); // Get base type name directly

    // Child 3: Dimension List (optional)
    ASTNode* dimListNode = typeNode->getRightSibling();
    currentArrayDimensions.clear(); // Reset dimensions for this variable
    bool isArray = false;
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        if (dimListNode->getLeftMostChild()) {
            isArray = true;
            dimListNode->accept(this); // Populates currentArrayDimensions
        }
    }

    // --- Symbol Handling ---
    // Check if this is a class attribute by examining the node's parent chain
    bool isClassAttribute = false;
    ASTNode* parent = node->getParent();
    
    // Check if this is coming from an Attribute node (indicates class member)
    if (parent && parent->getNodeEnum() == NodeType::ATTRIBUTE) {
        isClassAttribute = true;
    }

    // Find the appropriate table for this variable
    std::shared_ptr<SymbolTable> targetTable = currentTable;
    
    // If this is a class attribute but we're in the global table,
    // find the appropriate class table instead
    if (isClassAttribute && currentTable == symbolTable) {
        // Try to determine which class this attribute belongs to
        ASTNode* attributeNode = parent;
        ASTNode* memberNode = attributeNode ? attributeNode->getParent() : nullptr;
        ASTNode* memberListNode = memberNode ? memberNode->getParent() : nullptr;
        ASTNode* classNode = memberListNode ? memberListNode->getParent() : nullptr;
        ASTNode* classIdNode = classNode ? classNode->getLeftMostChild() : nullptr;
        
        if (classIdNode && classIdNode->getNodeEnum() == NodeType::CLASS_ID) {
            std::string className = classIdNode->getNodeValue();
            auto classTable = symbolTable->getNestedTable(className);
            if (classTable) {
                // Use the class table instead of currentTable
                targetTable = classTable;
            }
        }
        
        // If we couldn't determine the class table, keep the attribute in its
        // original table (probably a class table) but skip adding to global
        if (targetTable == symbolTable) {
            return; // Skip adding this attribute to the global table
        }
    }
    
    // Now handle the symbol in the appropriate table
    auto existingSymbol = targetTable->lookupSymbol(varName, true);
    if (existingSymbol) {
        // Re-add the symbol to update its position in the insertion order
        if (isArray) {
            existingSymbol->clearArrayDimensions();
            for(int dim : currentArrayDimensions) {
                existingSymbol->addArrayDimension(dim);
            }
        } else {
            existingSymbol->clearArrayDimensions();
        }
        targetTable->removeSymbol(varName);
        targetTable->addSymbol(existingSymbol);
    } else {
        // Create a new symbol only if it doesn't exist in the target table
        auto newSymbol = std::make_shared<Symbol>(varName, baseTypeName, SymbolKind::VARIABLE);
        if (isArray) {
            for(int dim : currentArrayDimensions) {
                newSymbol->addArrayDimension(dim);
            }
        }
        targetTable->addSymbol(newSymbol);
    }
    // --- End Symbol Handling ---
}

// visitParam is primarily handled by SymbolTableVisitor for symbol creation.
// MemSizeVisitor uses the symbol table populated by SymbolTableVisitor.
// We just need a basic traversal here if needed.
void MemSizeVisitor::visitParam(ASTNode* node) {
    // Basic traversal, symbol already exists from SymbolTableVisitor
    ASTNode* child = node->getLeftMostChild();
    while(child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

// visitAttribute just visits its child Variable node
void MemSizeVisitor::visitAttribute(ASTNode* node) {
    ASTNode* variableNode = node->getLeftMostChild();
    if (variableNode && variableNode->getNodeEnum() == NodeType::VARIABLE) {
        // Process the underlying variable declaration within the class scope
        variableNode->accept(this);
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
DEFAULT_VISITOR_METHOD(VariableId)
DEFAULT_VISITOR_METHOD(FunctionId)
DEFAULT_VISITOR_METHOD(FunctionSignature)
DEFAULT_VISITOR_METHOD(ConstructorSignature)
DEFAULT_VISITOR_METHOD(ParamList)
DEFAULT_VISITOR_METHOD(ParamId)
DEFAULT_VISITOR_METHOD(Type)
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
// DEFAULT_VISITOR_METHOD(DotAccess)
DEFAULT_VISITOR_METHOD(ArrayAccess)
DEFAULT_VISITOR_METHOD(Expr)
DEFAULT_VISITOR_METHOD(Factor)
DEFAULT_VISITOR_METHOD(AssignOp)
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

// Update visitArrayType to properly collect dimensions
void MemSizeVisitor::visitArrayType(ASTNode* node) {
    // Clear existing array dimensions
    currentArrayDimensions.clear();
    
    // Get type node (left child)
    ASTNode* typeNode = node->getLeftMostChild();
    if (typeNode) {
            // For simple types (e.g., int, float), just get the value
            currentType = typeNode->getNodeValue();
    }
    
    // Get dimension list node (right child)
    ASTNode* dimListNode = typeNode ? typeNode->getRightSibling() : nullptr;
    if (dimListNode && dimListNode->getNodeEnum() == NodeType::DIM_LIST) {
        // Process all dimensions in the list
        dimListNode->accept(this);
    } 
    // For backward compatibility
    else if (dimListNode && dimListNode->getNodeEnum() == NodeType::ARRAY_DIMENSION) {
        // Direct dimension node (legacy format)
        dimListNode->accept(this);
    }
}

// Add a proper implementation for visitArrayDimension
void MemSizeVisitor::visitArrayDimension(ASTNode* node) {
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
            // Handle invalid dimension (should log an error in a real implementation)
            currentArrayDimensions.push_back(1); // Default to 1
        } else {
            currentArrayDimensions.push_back(dimension);
        }
    } catch (const std::exception&) {
        // If not a number, default to dynamic array
        currentArrayDimensions.push_back(-1);
    }
}

void MemSizeVisitor::visitDimList(ASTNode* node) {
    // Clear existing array dimensions before processing
    currentArrayDimensions.clear();
    
    // Process each dimension node in the list
    ASTNode* dimNode = node->getLeftMostChild();
    while (dimNode) {
        // Process this dimension node
        dimNode->accept(this);
        
        // Move to next dimension
        dimNode = dimNode->getRightSibling();
    }
}

void MemSizeVisitor::visitIndexList(ASTNode* node) {
    // Process each index expression in the list
    ASTNode* indexNode = node->getLeftMostChild();
    int indexCount = 0;
    
    while (indexNode) {
        // Process this index expression - might create temp vars for complex expressions
        indexNode->accept(this);
        
        // If the index is a complex expression, we might need a temporary variable
        if (indexNode->getNodeEnum() != NodeType::INT && 
            indexNode->getNodeEnum() != NodeType::IDENTIFIER) {
            
            // Create a temp var to store the index value
            std::string tempVarName = createTempVar("int", "tempvar", indexNode);
            
            // Store the temp var name in the node's metadata for code generation
            indexNode->setMetadata("indexVar", tempVarName);
        }
        
        indexCount++;
        indexNode = indexNode->getRightSibling();
    }
    
    // Store the index count in the node's metadata
    node->setMetadata("indexCount", std::to_string(indexCount));
    
    // Create a temporary variable to store the total byte offset
    std::string totalOffsetVarName = createTempVar("int", "addrvar", node);
    
    // Store metadata about the byte offset variable
    node->setMetadata("byteOffsetVar", totalOffsetVarName);
    
    // If this is part of an array access, update the parent node's metadata
    if (node->getParent() && node->getParent()->getNodeEnum() == NodeType::ARRAY_ACCESS) {
        node->getParent()->setMetadata("indexCount", std::to_string(indexCount));
        node->getParent()->setMetadata("byteOffsetVar", totalOffsetVarName);
    }
}

void MemSizeVisitor::visitDotAccess(ASTNode* node) {
    // Process children first (object expression and member identifier)
    ASTNode* objExpr = node->getLeftMostChild();
    ASTNode* memberNode = objExpr ? objExpr->getRightSibling() : nullptr;

    if (objExpr) {
        objExpr->accept(this);
    }
    if (memberNode) {
        memberNode->accept(this);
    }

    // Determine the type of the member access
    std::string memberType = "int";  // Default type
    
    // Try to determine actual type from symbol table if possible
    if (objExpr && objExpr->getNodeEnum() == NodeType::IDENTIFIER) {
        std::string objName = objExpr->getNodeValue();
        auto objSymbol = currentTable->lookupSymbol(objName);
        
        if (objSymbol && memberNode) {
            std::string className = objSymbol->getType();
            std::string memberName = memberNode->getNodeValue();
            
            // Look up the member in the class table
            auto classTable = symbolTable->getNestedTable(className);
            if (classTable) {
                auto memberSymbol = classTable->lookupSymbol(memberName);
                if (memberSymbol) {
                    memberType = memberSymbol->getType();
                }
            }
        }
    }
    
    // Create a temporary variable to store the address of the member
    createTempVar("int", "addrvar", node);

    // Track the type for parent nodes
    node->setMetadata("type", memberType);
}