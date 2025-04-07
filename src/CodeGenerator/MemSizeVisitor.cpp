#include "MemSizeVisitor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Since we need to extend Symbol and SymbolTable with additional properties,
// we'll use custom metadata fields to store size and offset information

MemSizeVisitor::MemSizeVisitor(std::shared_ptr<SymbolTable> symbolTable)
    : symbolTable(symbolTable), tempVarCounter(1), currentTable(symbolTable) {
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

// Calculate the size of a type
int MemSizeVisitor::getTypeSize(const std::string& type) {
    // Basic types
    if (type == "int") return TypeSize::INT_SIZE;
    if (type == "float") return TypeSize::FLOAT_SIZE;
    if (type == "void") return TypeSize::VOID_SIZE;
    
    // Check for arrays
    size_t bracketPos = type.find('[');
    if (bracketPos != std::string::npos) {
        // Get base type (before brackets)
        std::string baseType = type.substr(0, bracketPos);
        int baseSize = getTypeSize(baseType);
        
        // Extract dimensions
        std::vector<int> dimensions;
        size_t start = bracketPos;
        while (start != std::string::npos) {
            size_t openBracket = type.find('[', start);
            if (openBracket == std::string::npos) break;
            
            size_t closeBracket = type.find(']', openBracket);
            if (closeBracket == std::string::npos) break;
            
            std::string dimStr = type.substr(openBracket + 1, closeBracket - openBracket - 1);
            try {
                if (!dimStr.empty()) {
                    dimensions.push_back(std::stoi(dimStr));
                } else {
                    dimensions.push_back(1); // Dynamic array assumed size 1 for calculation
                }
            } catch (const std::exception& e) {
                dimensions.push_back(1); // Default to 1 on error
            }
            
            start = closeBracket + 1;
        }
        
        // Calculate total array size
        int totalSize = baseSize;
        for (int dim : dimensions) {
            totalSize *= dim;
        }
        return totalSize;
    }
    
    // Check for user-defined classes
    auto classSymbol = symbolTable->lookupSymbol(type);
    if (classSymbol && classSymbol->getKind() == SymbolKind::CLASS) {
        // For classes, we'd need to sum up the size of all member variables
        // For simplicity, return a default size for user classes
        return TypeSize::POINTER_SIZE; // Pointer size for objects
    }
    
    // Default size for unknown types
    return TypeSize::POINTER_SIZE;
}

// Recursively calculate offsets for all symbols in a table
void MemSizeVisitor::calculateTableOffsets(std::shared_ptr<SymbolTable> table) {
    if (!table) return;
    
    int currentOffset = 0;
    
    // First handle parameters in declaration order using the function symbol
    auto funcSymbol = table->getFunctionSymbol();
    
    // Collect parameters in the order they were declared
    std::vector<std::shared_ptr<Symbol>> parameters;
    
    if (funcSymbol && funcSymbol->getKind() == SymbolKind::FUNCTION) {
        // Process parameters as before...
    }
    
    // Assign offsets to parameters in declaration order
    for (auto paramSymbol : parameters) {
        // Process parameters as before...
    }
    
    // Process variables in their original insertion order
    std::vector<std::shared_ptr<Symbol>> localVars;
    for (const auto& symbolName : table->getSymbolInsertionOrder()) {
        auto symbol = table->lookupSymbol(symbolName, true); // localOnly=true
        if (symbol && symbol->getKind() == SymbolKind::VARIABLE) {
            localVars.push_back(symbol);
        }
    }
    
    // Assign offsets to variables in reversed insertion order
    for (auto symbol : localVars) {
        int size = getTypeSize(symbol->getType());
        setSymbolSize(symbol, size);
        
        currentOffset -= size;
        setSymbolOffset(symbol, currentOffset);
    }
    
    // Save the total scope offset
    setTableScopeOffset(table, currentOffset);
    
    // Process nested tables (functions and classes)
    for (const auto& [name, nestedTable] : table->getNestedTables()) {
        calculateTableOffsets(nestedTable);
    }
}

std::string MemSizeVisitor::createTempVar(const std::string& type, const std::string& kind) {
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
        
        // Get current scope offset
        int currentOffset = getTableScopeOffset(currentTable);
        currentOffset -= size;
        setSymbolOffset(tempSymbol, currentOffset);
        
        // Update table scope offset
        setTableScopeOffset(currentTable, currentOffset);
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

void MemSizeVisitor::writeTableToFile(std::ofstream& out, std::shared_ptr<SymbolTable> table, int indent) {
    std::string indentStr(indent, ' ');
    int scopeOffset = getTableScopeOffset(table);
    
    // Print table header with scope offset
    out << indentStr << "========================================================" << std::endl;
    out << indentStr << "| table: " << std::left << std::setw(22) << table->getScopeName() 
        << "| scope offset: " << std::setw(7) << scopeOffset << " |" << std::endl;
    out << indentStr << "========================================================" << std::endl;
    
    // Collect all symbols (excluding classes and functions)
    std::vector<std::shared_ptr<Symbol>> allSymbols;
    for (const auto& symbolPair : table->getSymbolsInOrder()) {
        auto symbol = symbolPair.second;
        // Skip class and function symbols
        if (symbol->getKind() != SymbolKind::CLASS && 
            symbol->getKind() != SymbolKind::FUNCTION) {
            allSymbols.push_back(symbol);
        }
    }
    
    // Sort all symbols by their offset (ascending by absolute value)
    std::sort(allSymbols.begin(), allSymbols.end(), 
        [this](std::shared_ptr<Symbol> a, std::shared_ptr<Symbol> b) {
            return std::abs(getSymbolOffset(a)) < std::abs(getSymbolOffset(b));
        });
    
    // Print symbols in offset order
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
    
    // Collect nested tables in a vector to process them
    std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> nestedTables;
    for (const auto& tablePair : table->getNestedTables()) {
        nestedTables.push_back(tablePair);
    }
    
    // Reverse the order of nested tables
    std::reverse(nestedTables.begin(), nestedTables.end());
    
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
    
    currentFunctionName = funcIdNode->getNodeValue();
    
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
    
    // Build function table key (same logic as in SymbolTableVisitor)
    std::string tableKey = currentFunctionName;
    if (!paramTypes.empty()) {
        tableKey += "_";
        for (size_t i = 0; i < paramTypes.size(); i++) {
            if (i > 0) tableKey += "_";
            tableKey += paramTypes[i];
        }
    }
    
    // Find function table
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
        std::string type = (rightNode->getNodeEnum() == NodeType::FLOAT) ? "float" : "int";
        createTempVar(type, "litval");  // For direct literals, use "litval"
    }
    // Check if right-hand side is an operator (ADD_OP or MULT_OP)
    else if (rightNode && (rightNode->getNodeEnum() == NodeType::ADD_OP || 
                          rightNode->getNodeEnum() == NodeType::MULT_OP)) {
        // Process the operands first
        rightNode->accept(this);
        
        // Create a temporary for the operation result
        createTempVar("int", "tempvar");  // Operation results are "tempvar"
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
    // Generate a return value temp var if needed
    ASTNode* funcIdNode = node->getLeftMostChild();
    if (funcIdNode) {
        std::string funcName = funcIdNode->getNodeValue();
        auto funcSymbols = symbolTable->lookupFunctions(funcName);
        
        if (!funcSymbols.empty()) {
            std::string returnType = funcSymbols[0]->getType();
            if (returnType != "void") {
                createTempVar(returnType, "retval");
            }
        }
        
        // Process arguments - might generate temp vars
        ASTNode* argListNode = funcIdNode->getRightSibling();
        if (argListNode) {
            argListNode->accept(this);
        }
    }
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
    createTempVar("int", "tempvar"); // Boolean result represented as int
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
                createTempVar("int", "tempvar"); // Assuming int for simplicity
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
                createTempVar("int", "tempvar"); // Assuming int for simplicity
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
    if (!node->getParent() || (node->getParent()->getNodeEnum() != NodeType::ADD_OP && 
                              node->getParent()->getNodeEnum() != NodeType::MULT_OP)) {
        createTempVar("int", "litval");
    }
}

void MemSizeVisitor::visitFloat(ASTNode* node) {
    // Only create a temp var if this int is not part of an operation
    // Let ADD_OP and MULT_OP handle their operands
    if (!node->getParent() || (node->getParent()->getNodeEnum() != NodeType::ADD_OP && 
                              node->getParent()->getNodeEnum() != NodeType::MULT_OP)) {
        createTempVar("float", "litval");
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
    
    // Create a temporary variable for the result of the addition
    createTempVar("int", "tempvar");  // Adjust type as needed
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
    
    // Create a temporary variable for the result of the multiplication
    createTempVar("int", "tempvar");  // Adjust type as needed
}


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
DEFAULT_VISITOR_METHOD(Implementation)
DEFAULT_VISITOR_METHOD(ImplementationFunctionList)
DEFAULT_VISITOR_METHOD(Member)
DEFAULT_VISITOR_METHOD(Variable)
DEFAULT_VISITOR_METHOD(VariableId)
DEFAULT_VISITOR_METHOD(LocalVariable)
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
DEFAULT_VISITOR_METHOD(ArrayAccess)
DEFAULT_VISITOR_METHOD(Expr)
DEFAULT_VISITOR_METHOD(Factor)
DEFAULT_VISITOR_METHOD(AssignOp)
DEFAULT_VISITOR_METHOD(RelOp)
DEFAULT_VISITOR_METHOD(Attribute)
DEFAULT_VISITOR_METHOD(ImplementationId)