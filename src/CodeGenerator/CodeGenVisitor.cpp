#include "CodeGenVisitor.h"
#include <fstream>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

CodeGenVisitor::CodeGenVisitor(std::shared_ptr<SymbolTable> symbolTable) 
    : symbolTable(symbolTable), currentTable(symbolTable), 
      labelCounter(0), stringLiteralCounter(0) {  // Remove tempVarCounter
    
    // Initialize register pool with registers r1-r12 (r0, r13-r15 are reserved)
    for (int i = 12; i >= 1; i--) {
        registerPool.push(i);
    }
    
    // Initialize code sections
    dataSection = "% Data Section\n";
    dataSection += "          align\n";
    codeSection = "% Code Section\n";
}

CodeGenVisitor::~CodeGenVisitor() {
    // Clean up if necessary
}

// Process AST to generate code
void CodeGenVisitor::processAST(ASTNode* root) {
    if (!root) return;
    
    // Visit the AST
    root->accept(this);
    
    // End program
    codeSection += "          % End of program\n";
    codeSection += "          hlt\n";
    
    // Add buffer space for I/O
    dataSection += "% Buffer space for console I/O\n";
    dataSection += "buf       res 20\n";
}

// Output the generated code to a file
void CodeGenVisitor::generateOutputFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    outFile << codeSection << std::endl;
    outFile << dataSection << std::endl;
    
    outFile.close();
}

// Register allocation
int CodeGenVisitor::allocateRegister() {
    if (registerPool.empty()) {
        std::cerr << "Error: No registers available!" << std::endl;
        return -1;
    }
    
    int reg = registerPool.top();
    registerPool.pop();
    return reg;
}

void CodeGenVisitor::freeRegister(int reg) {
    // Skip r0, r13, r14, r15
    if (reg > 0 && reg < 13) {
        registerPool.push(reg);
    }
}

void CodeGenVisitor::freeAllRegisters() {
    while (!registerPool.empty()) {
        registerPool.pop();
    }
    for (int i = 12; i >= 1; i--) {
        registerPool.push(i);
    }
}

// Label generation
std::string CodeGenVisitor::generateLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(labelCounter++);
}

std::string CodeGenVisitor::generateStringLabel() {
    return "str_" + std::to_string(stringLiteralCounter++);
}

// Memory operations
void CodeGenVisitor::loadVariable(const std::string& varName, int targetReg) {
    auto varSymbol = currentTable->lookupSymbol(varName);
    if (!varSymbol) {
        std::cerr << "Error: Variable not found: " << varName << std::endl;
        return;
    }
    
    int offset = getSymbolOffset(varName);
    int size = getSymbolSize(varName);
    
    emit("% Load variable " + varName);
    if (size == 4) { // int
        emit("lw r" + std::to_string(targetReg) + "," + std::to_string(offset) + "(r14)");
    } else if (size == 8) { // float - requires special handling
        emit("lw r" + std::to_string(targetReg) + "," + std::to_string(offset) + "(r14)");
        emit("lw r" + std::to_string(targetReg + 1) + "," + std::to_string(offset + 4) + "(r14)");
    }
}

void CodeGenVisitor::storeVariable(const std::string& varName, int sourceReg) {
    auto varSymbol = currentTable->lookupSymbol(varName);
    if (!varSymbol) {
        std::cerr << "Error: Variable not found: " << varName << std::endl;
        return;
    }
    
    int offset = getSymbolOffset(varName);
    int size = getSymbolSize(varName);
    
    emit("% Store variable " + varName);
    if (size == 4) { // int
        emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(sourceReg));
    } else if (size == 8) { // float - requires special handling
        emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(sourceReg));
        emit("sw " + std::to_string(offset + 4) + "(r14),r" + std::to_string(sourceReg + 1));
    }
}

// Function handling
void CodeGenVisitor::generateFunctionPrologue(const std::string& funcName) {
    emitLabel(funcName);  // No indentation for function label
    emit("% Function prologue (" + funcName + ")");
    emit("sw -4(r14),r15");  // Save return address
}

void CodeGenVisitor::generateFunctionEpilogue() {
    emit("% Function epilogue");
    emit("lw r15,-4(r14)");  // Restore return address
    emit("jr r15");          // Return to caller
}

// Utility methods
int CodeGenVisitor::getSymbolOffset(const std::string& name) {
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol) {
        // Get offset from symbol metadata
        return std::stoi(symbol->getMetadata("offset"));
    }
    return 0; // Default
}

int CodeGenVisitor::getSymbolSize(const std::string& name) {
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol) {
        // Get size from symbol metadata
        return std::stoi(symbol->getMetadata("size"));
    }
    return 4; // Default to int size
}

std::string CodeGenVisitor::getSymbolTempVarKind(const std::string& name) {
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol) {
        // Get tempvar kind from symbol metadata
        return symbol->getMetadata("tempvar_kind");
    }
    return ""; // Default
}

int CodeGenVisitor::getScopeOffset(std::shared_ptr<SymbolTable> table) {
    // Get scope offset from metadata
    return std::stoi(table->getMetadata("scope_offset"));
}

void CodeGenVisitor::setScopeOffset(std::shared_ptr<SymbolTable> table, int offset) {
    // Set scope offset in metadata
    table->setMetadata("scope_offset", std::to_string(offset));
}

std::string CodeGenVisitor::formatInstructionWithComments(const std::string& instruction, const std::string& comment) {
    std::stringstream ss;
    ss << std::left << std::setw(10) << instruction;
    
    if (!comment.empty()) {
        ss << "% " << comment;
    }
    
    return ss.str();
}

// Code emission
void CodeGenVisitor::emit(const std::string& code) {
    codeSection += "          " + code + "\n";  // Indent instructions
}

void CodeGenVisitor::emitComment(const std::string& comment) {
    codeSection += "          % " + comment + "\n";  // Indent comments
}

void CodeGenVisitor::emitLabel(const std::string& label) {
    codeSection += label + "\n";  // No indentation for labels
}

// Basic visitor implementations with example implementations for common nodes

void CodeGenVisitor::visitProgram(ASTNode* node) {
    // Visit all children (class list, function list, implementation list)
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void CodeGenVisitor::visitBlock(ASTNode* node) {
    // Process each statement in the block
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

// Example implementation for integer literals
void CodeGenVisitor::visitInt(ASTNode* node) {
    // Allocate a register for this integer
    int reg = allocateRegister();
    
    // Get the integer value
    int value = std::stoi(node->getNodeValue());
    
    // Get metadata that was set by MemSizeVisitor
    std::string tempVarName = node->getMetadata("moonVarName");
    int offset = std::stoi(node->getMetadata("offset"));
    
    // Generate code to load the integer literal
    emitComment("processing: " + tempVarName + " := " + node->getNodeValue());
    emit("addi r" + std::to_string(reg) + ",r0," + std::to_string(value));
    
    // Store to temporary variable
    emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(reg));
    
    // Store register info for parent nodes
    node->setMetadata("reg", std::to_string(reg));
}

void CodeGenVisitor::visitFloat(ASTNode* node) {
    // Allocate a register for this float
    int reg = allocateRegister();
    
    // Get the float value
    float value = std::stof(node->getNodeValue());
    int intValue = static_cast<int>(value);
    
    // Get metadata that was set by MemSizeVisitor
    std::string tempVarName = node->getMetadata("moonVarName");
    int offset = std::stoi(node->getMetadata("offset"));
    
    // Generate code to load the float literal
    emitComment("processing: " + tempVarName + " := " + node->getNodeValue());
    emit("addi r" + std::to_string(reg) + ",r0," + std::to_string(intValue));
    
    // Store to temporary variable
    emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(reg));
    
    // Store register info for parent nodes
    node->setMetadata("reg", std::to_string(reg));
}

// Example implementation for addition operations
void CodeGenVisitor::visitAddOp(ASTNode* node) {
    // Traverse children first
    ASTNode* leftChild = node->getLeftMostChild();
    ASTNode* rightChild = leftChild ? leftChild->getRightSibling() : nullptr;
    
    if (leftChild) leftChild->accept(this);
    if (rightChild) rightChild->accept(this);
    
    // Get registers from child nodes
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    int reg3 = allocateRegister();
    
    // Get offsets for the operands - handle identifiers separately
    int leftOffset;
    int rightOffset;
    
    // Handle left operand
    if (leftChild->getNodeEnum() == NodeType::IDENTIFIER) {
        // For identifiers, get offset from symbol table
        leftOffset = getSymbolOffset(leftChild->getNodeValue());
    } else {
        // For expressions and literals, get from metadata
        leftOffset = std::stoi(leftChild->getMetadata("offset"));
    }
    
    // Handle right operand
    if (rightChild->getNodeEnum() == NodeType::IDENTIFIER) {
        // For identifiers, get offset from symbol table
        rightOffset = getSymbolOffset(rightChild->getNodeValue());
    } else {
        // For expressions and literals, get from metadata
        rightOffset = std::stoi(rightChild->getMetadata("offset"));
    }
    
    // Rest of the method remains unchanged
    // Get result metadata (already set by MemSizeVisitor)
    std::string resultVarName = node->getMetadata("moonVarName");
    int resultOffset = std::stoi(node->getMetadata("offset"));
    
    // Generate code for the operation
    std::string opStr = node->getNodeValue();
    emitComment("processing: " + resultVarName + " := " + 
             (leftChild->getNodeEnum() == NodeType::IDENTIFIER ? leftChild->getNodeValue() : leftChild->getMetadata("moonVarName")) + " " + opStr + " " + 
             (rightChild->getNodeEnum() == NodeType::IDENTIFIER ? rightChild->getNodeValue() : rightChild->getMetadata("moonVarName")));
    
    // Load values into registers
    emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
    emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
    
    // Perform addition/subtraction
    if (opStr == "+") {
        emit("add r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (opStr == "-") {
        emit("sub r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    
    // Store result in a temporary variable
    emit("sw " + std::to_string(resultOffset) + "(r14),r" + std::to_string(reg3));
    
    // Store register info for parent nodes
    node->setMetadata("reg", std::to_string(reg3));
    
    // Free registers
    freeRegister(reg1);
    freeRegister(reg2);
    // Don't free reg3 as parent will use it
}

void CodeGenVisitor::visitMultOp(ASTNode* node) {
    // Traverse children first
    ASTNode* leftChild = node->getLeftMostChild();
    ASTNode* rightChild = leftChild ? leftChild->getRightSibling() : nullptr;
    
    if (leftChild) leftChild->accept(this);
    if (rightChild) rightChild->accept(this);
    
    // Get registers
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    int reg3 = allocateRegister();
    
    // Get offsets for the operands - handle identifiers separately
    int leftOffset;
    int rightOffset;
    
    // Handle left operand
    if (leftChild->getNodeEnum() == NodeType::IDENTIFIER) {
        // For identifiers, get offset from symbol table
        leftOffset = getSymbolOffset(leftChild->getNodeValue());
    } else {
        // For expressions and literals, get from metadata
        leftOffset = std::stoi(leftChild->getMetadata("offset"));
    }
    
    // Handle right operand
    if (rightChild->getNodeEnum() == NodeType::IDENTIFIER) {
        // For identifiers, get offset from symbol table
        rightOffset = getSymbolOffset(rightChild->getNodeValue());
    } else {
        // For expressions and literals, get from metadata
        rightOffset = std::stoi(rightChild->getMetadata("offset"));
    }
    
    // Get result metadata (already set by MemSizeVisitor)
    std::string resultVarName = node->getMetadata("moonVarName");
    int resultOffset = std::stoi(node->getMetadata("offset"));
    
    // Generate code for the operation
    std::string opStr = node->getNodeValue();
    emitComment("processing: " + resultVarName + " := " + 
             (leftChild->getNodeEnum() == NodeType::IDENTIFIER ? leftChild->getNodeValue() : leftChild->getMetadata("moonVarName")) + " " + opStr + " " + 
             (rightChild->getNodeEnum() == NodeType::IDENTIFIER ? rightChild->getNodeValue() : rightChild->getMetadata("moonVarName")));
    
    // Load values into registers
    emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
    emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
    
    // Perform multiplication/division
    if (opStr == "*") {
        emit("mul r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (opStr == "/") {
        emit("div r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    
    // Store result in a temporary variable
    emit("sw " + std::to_string(resultOffset) + "(r14),r" + std::to_string(reg3));
    
    // Store register info for parent nodes
    node->setMetadata("reg", std::to_string(reg3));
    
    // Free registers
    freeRegister(reg1);
    freeRegister(reg2);
    // Don't free reg3 as parent will use it
}

// Example implementation for assignment
void CodeGenVisitor::visitAssignment(ASTNode* node) {
    // Get variable node and expression node
    ASTNode* varNode = node->getLeftMostChild();
    ASTNode* exprNode = varNode ? varNode->getRightSibling() : nullptr;
    
    if (!varNode || !exprNode) return;
    
    // Process the expression first
    exprNode->accept(this);
    
    // Only allocate one register - we don't need three
    int reg = allocateRegister();
    
    std::string varName = varNode->getNodeValue();
    std::string exprVarName = exprNode->getMetadata("moonVarName");
    
    // Get memory offsets
    int varOffset = getSymbolOffset(varName);
    int exprOffset = std::stoi(exprNode->getMetadata("offset"));
    
    // Generate assignment code
    emitComment("processing:: " + varName + " := " + exprVarName);
    emit("lw r" + std::to_string(reg) + "," + std::to_string(exprOffset) + "(r14)");
    emit("sw " + std::to_string(varOffset) + "(r14),r" + std::to_string(reg));
    
    // Free the register
    freeRegister(reg);
}

// Example default visitor method for nodes that just traverse children
void CodeGenVisitor::visitEmpty(ASTNode* node) {
    ASTNode* child = node->getLeftMostChild();
    while (child) {
        child->accept(this);
        child = child->getRightSibling();
    }
}

// Default implementation for most visitor methods
#define DEFAULT_VISITOR_METHOD(method) \
    void CodeGenVisitor::visit##method(ASTNode* node) { \
        ASTNode* child = node->getLeftMostChild(); \
        while (child) { \
            child->accept(this); \
            child = child->getRightSibling(); \
        } \
    }

// Generate default implementations for most AST nodes
DEFAULT_VISITOR_METHOD(ClassList)
DEFAULT_VISITOR_METHOD(FunctionList)
DEFAULT_VISITOR_METHOD(Implementation)
DEFAULT_VISITOR_METHOD(ImplementationList)
DEFAULT_VISITOR_METHOD(Class)
DEFAULT_VISITOR_METHOD(ClassId)
DEFAULT_VISITOR_METHOD(InheritanceList)
DEFAULT_VISITOR_METHOD(InheritanceId)
DEFAULT_VISITOR_METHOD(Visibility)
DEFAULT_VISITOR_METHOD(MemberList)
DEFAULT_VISITOR_METHOD(Member)
DEFAULT_VISITOR_METHOD(Variable)
DEFAULT_VISITOR_METHOD(FunctionId)
DEFAULT_VISITOR_METHOD(FunctionSignature)
DEFAULT_VISITOR_METHOD(FunctionBody)
DEFAULT_VISITOR_METHOD(ConstructorSignature)
DEFAULT_VISITOR_METHOD(LocalVariable)
DEFAULT_VISITOR_METHOD(IfStatement)
DEFAULT_VISITOR_METHOD(WhileStatement)
DEFAULT_VISITOR_METHOD(RelationalExpr)
DEFAULT_VISITOR_METHOD(FunctionDeclaration)
DEFAULT_VISITOR_METHOD(Attribute)
DEFAULT_VISITOR_METHOD(SingleStatement)
DEFAULT_VISITOR_METHOD(ExpressionStatement)
DEFAULT_VISITOR_METHOD(ReadStatement)
DEFAULT_VISITOR_METHOD(ReturnStatement)
DEFAULT_VISITOR_METHOD(AssignOp)
DEFAULT_VISITOR_METHOD(RelOp)
DEFAULT_VISITOR_METHOD(Identifier)
DEFAULT_VISITOR_METHOD(SelfIdentifier)
DEFAULT_VISITOR_METHOD(Type)
DEFAULT_VISITOR_METHOD(ArrayDimension)
DEFAULT_VISITOR_METHOD(Param)
DEFAULT_VISITOR_METHOD(FunctionCall)
DEFAULT_VISITOR_METHOD(ArrayAccess)
DEFAULT_VISITOR_METHOD(DotIdentifier)
DEFAULT_VISITOR_METHOD(DotAccess)
DEFAULT_VISITOR_METHOD(Factor)
DEFAULT_VISITOR_METHOD(Term)
DEFAULT_VISITOR_METHOD(ArithExpr)
DEFAULT_VISITOR_METHOD(Expr)
DEFAULT_VISITOR_METHOD(ImplementationId)
DEFAULT_VISITOR_METHOD(VariableId)
DEFAULT_VISITOR_METHOD(ParamList)
DEFAULT_VISITOR_METHOD(ParamId)
DEFAULT_VISITOR_METHOD(StatementsList)
DEFAULT_VISITOR_METHOD(ImplementationFunctionList)
DEFAULT_VISITOR_METHOD(Condition)
DEFAULT_VISITOR_METHOD(ArrayType)

void CodeGenVisitor::visitFunction(ASTNode* node) {
    // Get function signature (first child)
    ASTNode* functionSignature = node->getLeftMostChild();
    if (!functionSignature) return;
    
    // Get function ID from inside the signature
    ASTNode* functionId = functionSignature->getLeftMostChild();
    if (!functionId) return;
    
    std::string functionName = functionId->getNodeValue();
    currentFunction = functionName;
    
    codeSection+= "\n";
    emitComment("Processing function: " + functionName);
    
    // Save previous table before changing scope
    std::shared_ptr<SymbolTable> previousTable = currentTable;
    
    // Find and set the function's symbol table as current
    auto nestedTable = symbolTable->getNestedTable(functionName);
    if (nestedTable) {
        currentTable = nestedTable;
        emitComment("Entering function scope: " + functionName);
    }
    
    // Special handling for main function (entry point)
    if (functionName == "main") {
        emitLabel(functionName + "      entry");
        emit("addi r14,r0,topaddr");
    } else {
        // For non-main functions, generate prologue
        generateFunctionPrologue(functionName);
    }
    
    // Process function signature (parameters, etc.)
    functionSignature->accept(this);
    
    // Process function body (right sibling of signature)
    ASTNode* functionBody = functionSignature->getRightSibling();
    if (functionBody) {
        functionBody->accept(this);
    }
    
    // Generate function epilogue for non-main functions
    if (functionName != "main") {
        generateFunctionEpilogue();
    }
    
    // Restore previous table when leaving function scope
    currentTable = previousTable;
    emitComment("Exiting function scope: " + functionName);
}

void CodeGenVisitor::visitWriteStatement(ASTNode* node) {
    // Get the expression to print
    ASTNode* exprNode = node->getLeftMostChild();
    if (!exprNode) return;
    
    // Process the expression first
    exprNode->accept(this);
    
    // Allocate registers
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    
    // Get the output expression offset
    // int exprOffset = std::stoi(exprNode->getMetadata("offset"));
    // int scopeOffset = getScopeOffset(currentTable);
    
    // emitComment("processing: write(" + exprNode->getMetadata("moonVarName") + ")");
    // emit("lw r" + std::to_string(reg1) + "," + std::to_string(exprOffset) + "(r14)");
    
    // emitComment("put value on stack");
    // emit("addi r14,r14," + std::to_string(scopeOffset));
    // emit("sw -8(r14),r" + std::to_string(reg1));
    
    // emitComment("link buffer to stack");
    // emit("addi r" + std::to_string(reg1) + ",r0,buf");
    // emit("sw -12(r14),r" + std::to_string(reg1));
    
    // emitComment("convert int to string for output");
    // emit("jl r15,intstr");
    // emit("sw -8(r14),r13");
    
    // emitComment("output to console");
    // emit("jl r15,putstr");
    // emit("jl r15,putnl");  // add newline
    // emit("subi r14,r14," + std::to_string(scopeOffset));
    
    // Free registers
    freeRegister(reg1);
    freeRegister(reg2);
}

// int CodeGenVisitor::getNewTempVarOffset(const std::string& type) {
//     // Get the name of the current temporary variable
//     std::string tempVarName = "t" + std::to_string(tempVarCounter);
    
//     // Look up the temp var that was already created by MemSizeVisitor
//     auto tempVar = currentTable->lookupSymbol(tempVarName);
//     if (!tempVar) {
//         // Try to find any unused temp var as a fallback
//         for (int i = 1; i <= 10; i++) { // Check a reasonable number of temp vars
//             std::string altName = "t" + std::to_string(i);
//             auto alt = currentTable->lookupSymbol(altName);
//             if (alt) {
//                 std::cerr << "Warning: Using alternative temporary variable " 
//                           << altName << " instead of " << tempVarName << std::endl;
//                 tempVarCounter = i + 1; // Update counter for next time
//                 return getSymbolOffset(altName);
//             }
//         }
        
//         std::cerr << "Error: Temporary variable " << tempVarName 
//                   << " not found in symbol table!" << std::endl;
//         return 0; // Return a default offset
//     }
    
//     // Increment the counter for next use only after successful lookup
//     tempVarCounter++;
    
//     // Return the offset of the existing variable
//     return getSymbolOffset(tempVarName);
// }