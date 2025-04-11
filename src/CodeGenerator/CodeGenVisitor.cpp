#include "CodeGenVisitor.h"
#include <fstream>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

CodeGenVisitor::CodeGenVisitor(std::shared_ptr<SymbolTable> symbolTable) 
    : labelCounter(0), stringLiteralCounter(0) {
    
    // Create a deep copy of the symbol table
    this->symbolTable = std::make_shared<SymbolTable>(*symbolTable);
    
    // Set the current table to point to our copy of the symbol table
    currentTable = this->symbolTable;
    
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

    // Free the register
    freeRegister(reg);
}

void CodeGenVisitor::visitFloat(ASTNode* node) {
    // TODO: Handle float literals
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
    // Free the register
    freeRegister(reg);
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
    
    // Perform addition/subtraction/or
    if (opStr == "+") {
        emit("add r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (opStr == "-") {
        emit("sub r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (opStr == "or") {
        // Logical OR implementation
        std::string trueLabel = "logical_or_true_" + std::to_string(labelCounter);
        std::string endLabel = "logical_or_end_" + std::to_string(labelCounter++);
        
        emit("bnz r" + std::to_string(reg1) + "," + trueLabel); // If left operand is non-zero, result is 1
        emit("bnz r" + std::to_string(reg2) + "," + trueLabel); // If right operand is non-zero, result is 1
        emit("addi r" + std::to_string(reg3) + ",r0,0"); // Both are zero, result is 0
        emit("j " + endLabel);
        emitLabel(trueLabel);
        emit("addi r" + std::to_string(reg3) + ",r0,1"); // Result is 1
        emitLabel(endLabel);
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
    
    // Perform multiplication/division/and
    if (opStr == "*") {
        emit("mul r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (opStr == "/") {
        emit("div r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (opStr == "and") {
        // Logical AND implementation
        std::string zeroLabel = "logical_and_zero_" + std::to_string(labelCounter);
        std::string endLabel = "logical_and_end_" + std::to_string(labelCounter++);
        
        emit("bz r" + std::to_string(reg1) + "," + zeroLabel); // If left operand is 0, result is 0
        emit("bz r" + std::to_string(reg2) + "," + zeroLabel); // If right operand is 0, result is 0
        emit("addi r" + std::to_string(reg3) + ",r0,1"); // Both are non-zero, result is 1
        emit("j " + endLabel);
        emitLabel(zeroLabel);
        emit("addi r" + std::to_string(reg3) + ",r0,0"); // Result is 0
        emitLabel(endLabel);
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
// DEFAULT_VISITOR_METHOD(IfStatement)
// DEFAULT_VISITOR_METHOD(WhileStatement)
DEFAULT_VISITOR_METHOD(RelationalExpr)
DEFAULT_VISITOR_METHOD(FunctionDeclaration)
DEFAULT_VISITOR_METHOD(Attribute)
DEFAULT_VISITOR_METHOD(SingleStatement)
DEFAULT_VISITOR_METHOD(ExpressionStatement)
// DEFAULT_VISITOR_METHOD(ReturnStatement)
DEFAULT_VISITOR_METHOD(AssignOp)
// DEFAULT_VISITOR_METHOD(RelOp)
DEFAULT_VISITOR_METHOD(Identifier)
DEFAULT_VISITOR_METHOD(SelfIdentifier)
DEFAULT_VISITOR_METHOD(Type)
DEFAULT_VISITOR_METHOD(ArrayDimension)
DEFAULT_VISITOR_METHOD(Param)
// DEFAULT_VISITOR_METHOD(FunctionCall)
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
// DEFAULT_VISITOR_METHOD(Condition)
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
    // Fix the table lookup to use the correct naming convention with parameter types
    auto funcSymbols = symbolTable->lookupFunctions(functionName);
    std::shared_ptr<SymbolTable> nestedTable = nullptr;

    if (!funcSymbols.empty()) {
        // Format the unique key using the same convention as in SymbolTableVisitor
        std::string uniqueKey = functionName;
        const auto& paramTypes = funcSymbols[0]->getParams();
        
        if (!paramTypes.empty()) {
            uniqueKey += "_";
            for (size_t i = 0; i < paramTypes.size(); ++i) {
                // Replace any special characters that might cause issues in table names
                std::string cleanType = paramTypes[i];
                std::replace(cleanType.begin(), cleanType.end(), '[', '_');
                std::replace(cleanType.begin(), cleanType.end(), ']', '_');
                std::replace(cleanType.begin(), cleanType.end(), ' ', '_');
                
                uniqueKey += cleanType;
                if (i < paramTypes.size() - 1) {
                    uniqueKey += "_";
                }
            }
        }
        
        // Now try to get the table with the formatted key
        nestedTable = symbolTable->getNestedTable(uniqueKey);
        
        if (!nestedTable) {
            // Fallback: try with just the function name (for simple cases)
            nestedTable = symbolTable->getNestedTable(functionName);
        }
    } else {
        // Fallback to just the function name if no symbols found
        nestedTable = symbolTable->getNestedTable(functionName);
    }

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
    
    // Get the output expression offset - handle identifiers correctly
    int exprOffset;
    if (exprNode->getNodeEnum() == NodeType::IDENTIFIER) {
        // For identifiers, get offset from symbol table
        exprOffset = getSymbolOffset(exprNode->getNodeValue());
    } else {
        // For expressions and literals, get from metadata
        exprOffset = std::stoi(exprNode->getMetadata("offset"));
    }
    
    int scopeOffset = getScopeOffset(currentTable);
    
    // Get the variable name for the comment
    std::string varName = (exprNode->getNodeEnum() == NodeType::IDENTIFIER) ? 
                          exprNode->getNodeValue() : 
                          exprNode->getMetadata("moonVarName");
    
    emitComment("processing: write(" + varName + ")");
    emit("lw r" + std::to_string(reg1) + "," + std::to_string(exprOffset) + "(r14)");
    
    // Rest of the method remains unchanged
    emitComment("put value on stack");
    emit("addi r14,r14," + std::to_string(scopeOffset));
    emit("sw -8(r14),r" + std::to_string(reg1));
    
    emitComment("link buffer to stack");
    emit("addi r" + std::to_string(reg1) + ",r0,buf");
    emit("sw -12(r14),r" + std::to_string(reg1));
    
    emitComment("convert int to string for output");
    emit("jl r15,intstr");
    emit("sw -8(r14),r13");
    
    emitComment("output to console");
    emit("jl r15,putstr");
    emit("jl r15,putnl");  // add newline
    emit("subi r14,r14," + std::to_string(scopeOffset));
    
    // Free registers
    freeRegister(reg1);
    freeRegister(reg2);
}

void CodeGenVisitor::visitReadStatement(ASTNode* node) {
    // Get the variable to read into
    ASTNode* varNode = node->getLeftMostChild();
    if (!varNode) return;
    
    // Allocate registers
    int reg1 = allocateRegister();
    
    // Get the variable offset - handle identifiers
    int varOffset;
    if (varNode->getNodeEnum() == NodeType::IDENTIFIER) {
        // For identifiers, get offset from symbol table
        varOffset = getSymbolOffset(varNode->getNodeValue());
    } else {
        // For expressions, get from metadata
        varOffset = std::stoi(varNode->getMetadata("offset"));
    }
    
    int scopeOffset = getScopeOffset(currentTable);
    
    // Get the variable name for the comment
    std::string varName = (varNode->getNodeEnum() == NodeType::IDENTIFIER) ? 
                          varNode->getNodeValue() : 
                          varNode->getMetadata("moonVarName");
    
    emitComment("Processing: read(" + varName + ")");
    
    // Allocate buffer space (could be in data section)
    emitComment("Allocate buffer space for input");
    emit("addi r" + std::to_string(reg1) + ",r0,buf");
    emit("sw -8(r14),r" + std::to_string(reg1));
    
    // Read string from stdin
    emitComment("Read string from console");
    emit("addi r14,r14," + std::to_string(scopeOffset));
    emit("jl r15,getstr");
    emit("subi r14,r14," + std::to_string(scopeOffset));
    
    // Convert string to integer
    emitComment("Convert string to integer");
    emit("addi r" + std::to_string(reg1) + ",r0,buf");
    emit("sw -8(r14),r" + std::to_string(reg1));
    
    emit("addi r14,r14," + std::to_string(scopeOffset));
    emit("jl r15,strint");
    emit("subi r14,r14," + std::to_string(scopeOffset));
    
    // Now r13 contains the integer value
    emitComment("Store read value into r13");
    emit("sw " + std::to_string(varOffset) + "(r14),r13");
    
    // Free registers
    freeRegister(reg1);
}

void CodeGenVisitor::visitIfStatement(ASTNode* node) {
    // Create unique labels for branching
    std::string thenLabel = "then_" + std::to_string(labelCounter++);
    std::string elseLabel = "else_" + std::to_string(labelCounter++);
    std::string endifLabel = "endif_" + std::to_string(labelCounter++);
    
    // Get the three children: condition, then-block, and else-block
    ASTNode* conditionNode = node->getLeftMostChild();
    ASTNode* thenBlock = conditionNode ? conditionNode->getRightSibling() : nullptr;
    ASTNode* elseBlock = thenBlock ? thenBlock->getRightSibling() : nullptr;
    
    emitComment("if statement - evaluating condition");
    
    // Process condition and get its result
    if (conditionNode) {
        conditionNode->accept(this);
        
        // Get the register or memory location holding the condition result
        int condReg = allocateRegister();
        int condOffset;
        
        if (conditionNode->getMetadata("reg").empty()) {
            // Condition result is in memory, load it
            condOffset = std::stoi(conditionNode->getMetadata("offset"));
            emit("lw r" + std::to_string(condReg) + "," + std::to_string(condOffset) + "(r14)");
        } else {
            // Condition result is already in a register
            int sourceReg = std::stoi(conditionNode->getMetadata("reg"));
            emit("add r" + std::to_string(condReg) + ",r" + std::to_string(sourceReg) + ",r0");
        }
        
        // Branch based on condition (0 = false, non-zero = true)
        emit("bz r" + std::to_string(condReg) + "," + elseLabel);
        freeRegister(condReg);
    }
    
    // Then block
    emitComment("then branch");
    emitLabel(thenLabel);
    if (thenBlock) {
        thenBlock->accept(this);
    }
    emit("j " + endifLabel);
    
    // Else block
    emitComment("else branch");
    emitLabel(elseLabel);
    if (elseBlock) {
        elseBlock->accept(this);
    }
    
    // End of if statement
    emitLabel(endifLabel);
    emitComment("end if");
}

void CodeGenVisitor::visitCondition(ASTNode* node) {
    // A condition has a rel_op node as its only child
    ASTNode* relOpNode = node->getLeftMostChild();
    if (!relOpNode) return;
    
    // Process the rel_op node
    relOpNode->accept(this);
    
    // Pass register info up to parent node
    if (!relOpNode->getMetadata("reg").empty()) {
        node->setMetadata("reg", relOpNode->getMetadata("reg"));
    }
    
    // Pass memory location info up to parent node
    if (!relOpNode->getMetadata("offset").empty()) {
        node->setMetadata("offset", relOpNode->getMetadata("offset"));
    }
}

void CodeGenVisitor::visitRelOp(ASTNode* node) {
    // Get the left and right arithmetic expressions
    ASTNode* leftExpr = node->getLeftMostChild();
    ASTNode* rightExpr = leftExpr ? leftExpr->getRightSibling() : nullptr;
    
    if (!leftExpr || !rightExpr) return;
    
    // Process operands
    leftExpr->accept(this);
    rightExpr->accept(this);
    
    // Allocate registers for the comparison
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    int resultReg = allocateRegister();
    
    // Get operand values into registers - handle identifiers separately
    int leftOffset, rightOffset;
    
    // Handle left operand
    if (leftExpr->getNodeEnum() == NodeType::IDENTIFIER) {
        leftOffset = getSymbolOffset(leftExpr->getNodeValue());
    } else {
        leftOffset = std::stoi(leftExpr->getMetadata("offset"));
    }
    
    // Handle right operand
    if (rightExpr->getNodeEnum() == NodeType::IDENTIFIER) {
        rightOffset = getSymbolOffset(rightExpr->getNodeValue());
    } else {
        rightOffset = std::stoi(rightExpr->getMetadata("offset"));
    }
    
    // Load operands into registers
    emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
    emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
    
    // Get the relational operator
    std::string op = node->getNodeValue();
    
    // Use Moon's comparison instructions instead of branch instructions
    if (op == ">") {
        emit("cgt r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (op == ">=") {
        emit("cge r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (op == "<") {
        emit("clt r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (op == "<=") {
        emit("cle r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (op == "==") {
        emit("ceq r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    } else if (op == "<>") {
        emit("cne r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    
    // Store the result - result is already 1 (true) or 0 (false)
    int resultOffset = std::stoi(node->getMetadata("offset"));
    emit("sw " + std::to_string(resultOffset) + "(r14),r" + std::to_string(resultReg));
    node->setMetadata("reg", std::to_string(resultReg));
    
    // Free registers for operands
    freeRegister(reg1);
    freeRegister(reg2);
}

void CodeGenVisitor::visitWhileStatement(ASTNode* node) {
    // Create unique labels for the loop
    std::string whileStartLabel = "while_start_" + std::to_string(labelCounter++);
    std::string whileEndLabel = "while_end_" + std::to_string(labelCounter++);
    
    // Get the condition and body blocks
    ASTNode* conditionNode = node->getLeftMostChild();
    ASTNode* bodyNode = conditionNode ? conditionNode->getRightSibling() : nullptr;
    
    // Label for the start of the loop (condition evaluation)
    emitLabel(whileStartLabel);
    emitComment("while loop - evaluating condition");
    
    // Process the condition
    if (conditionNode) {
        conditionNode->accept(this);
        
        // Get the register or memory location holding the condition result
        int condReg = allocateRegister();
        int condOffset;
        
        if (conditionNode->getMetadata("reg").empty()) {
            // Condition result is in memory, load it
            condOffset = std::stoi(conditionNode->getMetadata("offset"));
            emit("lw r" + std::to_string(condReg) + "," + std::to_string(condOffset) + "(r14)");
        } else {
            // Condition result is already in a register
            int sourceReg = std::stoi(conditionNode->getMetadata("reg"));
            emit("add r" + std::to_string(condReg) + ",r" + std::to_string(sourceReg) + ",r0");
        }
        
        // Branch to end if condition is false (0)
        emit("bz r" + std::to_string(condReg) + "," + whileEndLabel);
        freeRegister(condReg);
    }
    
    // Process the loop body
    emitComment("while loop - executing body");
    if (bodyNode) {
        bodyNode->accept(this);
    }
    
    // Jump back to the start of the loop
    emit("j " + whileStartLabel);
    
    // Label for the end of the loop
    emitLabel(whileEndLabel);
    emitComment("end while loop");
}

void CodeGenVisitor::visitFunctionCall(ASTNode* node) {
    // Get function name and parameter list
    ASTNode* idNode = node->getLeftMostChild();
    ASTNode* paramListNode = idNode ? idNode->getRightSibling() : nullptr;
    
    if (!idNode) return;
    
    std::string functionName = idNode->getNodeValue();
    
    // Generate comment for function call
    emitComment("Function call: " + functionName);
    
    // Save current scope offset for restoring later
    int currentScopeOffset = getScopeOffset(currentTable);
    
    // Step 1: Process parameters in reverse order (right to left)
    std::vector<ASTNode*> params;
    if (paramListNode) {
        ASTNode* paramNode = paramListNode->getLeftMostChild();
        while (paramNode) {
            params.push_back(paramNode);
            paramNode = paramNode->getRightSibling();
        }
    }
    
    // Step 2: Evaluate parameters and push them onto the stack
    emitComment("Pushing parameters onto stack");
        
    // First, find function to determine return type size
    auto funcSymbols = symbolTable->lookupFunctions(functionName);

    int paramOffset = currentScopeOffset;

    // Process parameters in reverse order
    for (auto it = params.rbegin(); it != params.rend(); ++it) {
        ASTNode* param = *it;
        
        // Visit the parameter to evaluate it
        param->accept(this);
        
        // Get parameter value
        int paramReg = allocateRegister();
        int offset;
        // Load parameter value (could be from variable or temp)
        if (param->getNodeEnum() == NodeType::IDENTIFIER) {
            offset = getSymbolOffset(param->getNodeValue());
            emit("lw r" + std::to_string(paramReg) + "," + std::to_string(offset) + "(r14)");
        } else {
            // Expression result is in a temporary variable
            offset = std::stoi(param->getMetadata("offset"));
            emit("lw r" + std::to_string(paramReg) + "," + std::to_string(offset) + "(r14)");
        }
        // Push parameter onto the stack at the correct offset
        emit("sw " + std::to_string(paramOffset+offset) + "(r14),r" + std::to_string(paramReg));
        
        
        freeRegister(paramReg);
    }

    // Adjust frame pointer for function call - total offset includes parameters and reserved space
    emitComment("Setting up activation record for function call");
    emit("addi r14,r14," + std::to_string(paramOffset));
    
    // Step 4: Jump to function with link
    emit("jl r15," + functionName);
    
    // Step 5: Restore stack frame after function returns
    emitComment("Function " + functionName + " returned, restoring stack");
    emit("subi r14,r14," + std::to_string(paramOffset));
    
    // Step 6: Handle return value if the function returns something
    // Look up function in symbol table to determine its return type
    //auto funcSymbols = symbolTable->lookupFunctions(functionName)[0];
    if (!funcSymbols.empty() && funcSymbols[0]->getType() != "void") {
        // For non-void functions, the return value is in r13
        
        // Get temporary variable name from metadata if available
        std::string retVarName;
        int retOffset;
        
        if (!node->getMetadata("moonVarName").empty()) {
            retVarName = node->getMetadata("moonVarName");
            
            // Look up the symbol in the current table
            auto tempSymbol = currentTable->lookupSymbol(retVarName);
            if (tempSymbol) {
                retOffset = getSymbolOffset(retVarName);
            } else {
                // Fallback: check if offset is directly available
                if (!node->getMetadata("offset").empty()) {
                    retOffset = std::stoi(node->getMetadata("offset"));
                } else {
                    // Emergency fallback - create a temporary location in the stack
                    emitComment("Warning: No temp var found for return value");
                    retOffset = -8; // Use a common temporary area
                }
            }
        } else if (!node->getMetadata("offset").empty()) {
            // Direct offset available
            retOffset = std::stoi(node->getMetadata("offset"));
        } else {
            // No metadata available
            emitComment("Warning: No temp var found for return value");
            retOffset = -8; // Use a common temporary area
        }
        
        emitComment("Storing return value from r13");
        emit("sw " + std::to_string(retOffset) + "(r14),r13");
        
        // Set register metadata for parent nodes
        node->setMetadata("reg", "13"); // Return value is in r13
    }
}

void CodeGenVisitor::visitReturnStatement(ASTNode* node) {
    // Get the return value expression, if any
    ASTNode* exprNode = node->getLeftMostChild();
    if (exprNode) {
        // Process the expression to evaluate it
        exprNode->accept(this);
        
        // Get current function
        std::string funcName = currentFunction;
        auto funcSymbol = symbolTable->lookupFunctions(funcName)[0];
        
        // Get return value offset
        int returnOffset = -8; // Default if metadata not found
        if (funcSymbol && !funcSymbol->getMetadata("return_offset").empty()) {
            returnOffset = std::stoi(funcSymbol->getMetadata("return_offset"));
        }
        
        // Load return value into register
        int retReg = allocateRegister();
        
        // Handle different types of return expressions
        if (exprNode->getNodeEnum() == NodeType::IDENTIFIER) {
            int valOffset = getSymbolOffset(exprNode->getNodeValue());
            emit("lw r" + std::to_string(retReg) + "," + std::to_string(valOffset) + "(r14)");
        } else {
            // Expression result is in a temporary variable
            int valOffset = std::stoi(exprNode->getMetadata("offset"));
            emit("lw r" + std::to_string(retReg) + "," + std::to_string(valOffset) + "(r14)");
        }
        
        // Store the value in the designated return area
        emit("sw " + std::to_string(returnOffset) + "(r14),r" + std::to_string(retReg));
        
        // Also put the return value in r13 by convention
        emit("add r13,r" + std::to_string(retReg) + ",r0");
        
        freeRegister(retReg);
    }
    
    // Get the link register offset
    std::string funcName = currentFunction;
    auto funcSymbol = symbolTable->lookupFunctions(funcName)[0];
    int linkOffset = -4; // Default
    
    if (funcSymbol && !funcSymbol->getMetadata("link_register_offset").empty()) {
        linkOffset = std::stoi(funcSymbol->getMetadata("link_register_offset"));
    }
    
    // Restore return address and return
    emit("lw r15," + std::to_string(linkOffset) + "(r14)");
    emit("jr r15");
}
