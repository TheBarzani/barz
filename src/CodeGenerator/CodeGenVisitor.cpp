#include "CodeGenVisitor.h"
#include <fstream>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

CodeGenVisitor::CodeGenVisitor(std::shared_ptr<SymbolTable> symbolTable) 
    : symbolTable(symbolTable), currentTable(symbolTable), 
      labelCounter(0), stringLiteralCounter(0) {
    
    // Initialize register allocation tracking
    for (int i = 0; i < NUM_REGS; i++) {
        registerUsed[i] = false;
    }
    
    // Reserve r0 (always 0), r13 (return value for IO), r14 (stack frame pointer), and r15 (return address)
    registerUsed[0] = true;
    registerUsed[13] = true;
    registerUsed[14] = true;
    registerUsed[15] = true;
    
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
    // Find first available register (skip r0, r14, r15)
    for (int i = 1; i < NUM_REGS; i++) {
        if (i != 14 && i != 15 && !registerUsed[i]) {
            registerUsed[i] = true;
            return i;
        }
    }
    
    // No registers available
    std::cerr << "Error: No registers available!" << std::endl;
    return -1;
}

void CodeGenVisitor::freeRegister(int reg) {
    if (reg > 0 && reg < NUM_REGS && reg != 14 && reg != 15) {
        registerUsed[reg] = false;
    }
}

void CodeGenVisitor::freeAllRegisters() {
    for (int i = 1; i < NUM_REGS; i++) {
        if (i != 14 && i != 15 && i != 13) {
            registerUsed[i] = false;
        }
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
    emit(funcName + ":");
    emit("% Function prologue");
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
    codeSection += "          " + code + "\n";
}

void CodeGenVisitor::emitComment(const std::string& comment) {
    codeSection += "          % " + comment + "\n";
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
    int value = std::stoi(node->getNodeValue());
    int reg = allocateRegister();
    
    // Generate code to load the integer value
    emit("% Load integer literal: " + std::to_string(value));
    emit("addi r" + std::to_string(reg) + ",r0," + std::to_string(value));
    
    // Save result in expression stack
    exprStack.push(reg);
}

// Example implementation for addition operations
void CodeGenVisitor::visitAddOp(ASTNode* node) {
    // Visit left and right operands
    ASTNode* leftOperand = node->getLeftMostChild();
    if (leftOperand) {
        leftOperand->accept(this);
    }
    
    ASTNode* rightOperand = leftOperand ? leftOperand->getRightSibling() : nullptr;
    if (rightOperand) {
        rightOperand->accept(this);
    }
    
    // Get values from expression stack
    int rightReg = exprStack.top(); exprStack.pop();
    int leftReg = exprStack.top(); exprStack.pop();
    
    // Allocate a new register for the result
    int resultReg = allocateRegister();
    
    // Generate code for addition
    std::string opStr = node->getNodeValue();
    if (opStr == "+") {
        emit("% Addition operation");
        emit("add r" + std::to_string(resultReg) + ",r" + std::to_string(leftReg) + ",r" + std::to_string(rightReg));
    } else if (opStr == "-") {
        emit("% Subtraction operation");
        emit("sub r" + std::to_string(resultReg) + ",r" + std::to_string(leftReg) + ",r" + std::to_string(rightReg));
    }
    
    // Free operand registers
    freeRegister(leftReg);
    freeRegister(rightReg);
    
    // Push result register
    exprStack.push(resultReg);
}

// Example implementation for assignment
void CodeGenVisitor::visitAssignment(ASTNode* node) {
    // Get variable identifier and expression
    ASTNode* varNode = node->getLeftMostChild();
    ASTNode* exprNode = varNode ? varNode->getRightSibling() : nullptr;
    
    if (!varNode || !exprNode) return;
    
    // Evaluate the expression first
    exprNode->accept(this);
    
    // Get result register from expression stack
    int resultReg = exprStack.top(); exprStack.pop();
    
    // Store result in variable
    std::string varName = varNode->getNodeValue();
    emitComment("Assignment to " + varName);
    storeVariable(varName, resultReg);
    
    // Free the result register
    freeRegister(resultReg);
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
DEFAULT_VISITOR_METHOD(WriteStatement)
DEFAULT_VISITOR_METHOD(ReturnStatement)
DEFAULT_VISITOR_METHOD(AssignOp)
DEFAULT_VISITOR_METHOD(RelOp)
DEFAULT_VISITOR_METHOD(MultOp)
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
DEFAULT_VISITOR_METHOD(Float)
DEFAULT_VISITOR_METHOD(Function)