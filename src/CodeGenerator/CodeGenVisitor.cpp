#include "CodeGenVisitor.h"
#include <fstream>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

CodeGenVisitor::CodeGenVisitor(std::shared_ptr<SymbolTable> symbolTable)
    : labelCounter(0), stringLiteralCounter(0), currentTotalScopeOffset(0)
{

    // Create a deep copy of the symbol table
    this->symbolTable = std::make_shared<SymbolTable>(*symbolTable);

    // Set the current table to point to our copy of the symbol table
    currentTable = this->symbolTable;

    // Initialize register pool with registers r1-r12 (r0, r13-r15 are reserved)
    for (int i = 12; i >= 1; i--)
    {
        registerPool.push(i);
    }

    // Initialize code sections
    dataSection = "% Data Section\n";
    dataSection += "          align\n";
    codeSection = "% Code Section\n";
}

CodeGenVisitor::~CodeGenVisitor()
{
    // Clean up if necessary
}

// Process AST to generate code
void CodeGenVisitor::processAST(ASTNode *root)
{
    if (!root)
        return;

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
void CodeGenVisitor::generateOutputFile(const std::string &filename)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }

    outFile << codeSection << std::endl;
    outFile << dataSection << std::endl;

    outFile.close();
}

// Register allocation
int CodeGenVisitor::allocateRegister()
{
    if (registerPool.empty())
    {
        std::cerr << "Error: No registers available!" << std::endl;
        return -1;
    }

    int reg = registerPool.top();
    registerPool.pop();
    return reg;
}

void CodeGenVisitor::freeRegister(int reg)
{
    // Skip r0, r13, r14, r15
    if (reg > 0 && reg < 13)
    {
        registerPool.push(reg);
    }
}

void CodeGenVisitor::freeAllRegisters()
{
    while (!registerPool.empty())
    {
        registerPool.pop();
    }
    for (int i = 12; i >= 1; i--)
    {
        registerPool.push(i);
    }
}

// Label generation
std::string CodeGenVisitor::generateLabel(const std::string &prefix)
{
    return prefix + "_" + std::to_string(labelCounter++);
}

std::string CodeGenVisitor::generateStringLabel()
{
    return "str_" + std::to_string(stringLiteralCounter++);
}

// Memory operations
void CodeGenVisitor::loadVariable(const std::string &varName, int targetReg)
{
    auto varSymbol = currentTable->lookupSymbol(varName);
    if (!varSymbol)
    {
        std::cerr << "Error: Variable not found: " << varName << std::endl;
        return;
    }

    int offset = getSymbolOffset(varName);
    int size = getSymbolSize(varName);

    emit("% Load variable " + varName);
    if (size == 4)
    { // int
        emit("lw r" + std::to_string(targetReg) + "," + std::to_string(offset) + "(r14)");
    }
    else if (size == 8)
    { // float - requires special handling
        emit("lw r" + std::to_string(targetReg) + "," + std::to_string(offset) + "(r14)");
        emit("lw r" + std::to_string(targetReg + 1) + "," + std::to_string(offset + 4) + "(r14)");
    }
}

void CodeGenVisitor::storeVariable(const std::string &varName, int sourceReg)
{
    auto varSymbol = currentTable->lookupSymbol(varName);
    if (!varSymbol)
    {
        std::cerr << "Error: Variable not found: " << varName << std::endl;
        return;
    }

    int offset = getSymbolOffset(varName);
    int size = getSymbolSize(varName);

    emit("% Store variable " + varName);
    if (size == 4)
    { // int
        emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(sourceReg));
    }
    else if (size == 8)
    { // float - requires special handling
        emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(sourceReg));
        emit("sw " + std::to_string(offset + 4) + "(r14),r" + std::to_string(sourceReg + 1));
    }
}

// Function handling
void CodeGenVisitor::generateFunctionPrologue(const std::string &funcName)
{
    emitLabel(funcName); // No indentation for function label
    emit("% Function prologue (" + funcName + ")");
    emit("sw -4(r14),r15"); // Save return address
}

void CodeGenVisitor::generateFunctionEpilogue()
{
    emit("% Function epilogue");
    emit("lw r15,-4(r14)"); // Restore return address
    emit("jr r15");         // Return to caller
}

// Utility methods
int CodeGenVisitor::getSymbolOffset(const std::string &name)
{
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol)
    {
        // Get offset from symbol metadata
        return std::stoi(symbol->getMetadata("offset"));
    }
    return 0; // Default
}

int CodeGenVisitor::getSymbolSize(const std::string &name)
{
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol)
    {
        // Get size from symbol metadata
        return std::stoi(symbol->getMetadata("size"));
    }
    return 4; // Default to int size
}

std::string CodeGenVisitor::getSymbolTempVarKind(const std::string &name)
{
    auto symbol = currentTable->lookupSymbol(name);
    if (symbol)
    {
        // Get tempvar kind from symbol metadata
        return symbol->getMetadata("tempvar_kind");
    }
    return ""; // Default
}

int CodeGenVisitor::getScopeOffset(std::shared_ptr<SymbolTable> table)
{
    // Get scope offset from metadata
    return std::stoi(table->getMetadata("scope_offset"));
}

void CodeGenVisitor::setScopeOffset(std::shared_ptr<SymbolTable> table, int offset)
{
    // Set scope offset in metadata
    table->setMetadata("scope_offset", std::to_string(offset));
}

std::string CodeGenVisitor::formatInstructionWithComments(const std::string &instruction, const std::string &comment)
{
    std::stringstream ss;
    ss << std::left << std::setw(10) << instruction;

    if (!comment.empty())
    {
        ss << "% " << comment;
    }

    return ss.str();
}

// Code emission
void CodeGenVisitor::emit(const std::string &code)
{
    codeSection += "          " + code + "\n"; // Indent instructions
}

void CodeGenVisitor::emitComment(const std::string &comment)
{
    codeSection += "          % " + comment + "\n"; // Indent comments
}

void CodeGenVisitor::emitLabel(const std::string &label)
{
    codeSection += label + "\n"; // No indentation for labels
}

// Basic visitor implementations with example implementations for common nodes

void CodeGenVisitor::visitProgram(ASTNode *node)
{
    // Visit all children (class list, function list, implementation list)
    ASTNode *child = node->getLeftMostChild();
    while (child)
    {
        child->accept(this);
        child = child->getRightSibling();
    }
}

void CodeGenVisitor::visitBlock(ASTNode *node)
{
    // Process each statement in the block
    ASTNode *child = node->getLeftMostChild();
    while (child)
    {
        child->accept(this);
        child = child->getRightSibling();
    }
}

// Example implementation for integer literals
void CodeGenVisitor::visitInt(ASTNode *node)
{
    // Allocate a register for this integer
    int reg = allocateRegister();

    // Get the integer value
    int value = std::stoi(node->getNodeValue());

    // Get metadata that was set by MemSizeVisitor
    std::string tempVarName = node->getMetadata("moonVarName");
    // Look up symbol in the table to get the correct offset
    int offset;
    auto symbol = currentTable->lookupSymbol(tempVarName);
    if (symbol && !symbol->getMetadata("offset").empty())
    {
        offset = std::stoi(symbol->getMetadata("offset"));
    }
    else if (!node->getMetadata("offset").empty())
    {
        // Fallback: use offset from node metadata if symbol not found
        offset = std::stoi(node->getMetadata("offset"));
        emitComment("Warning: Symbol for " + tempVarName + " not found, using node metadata offset");
    }
    else
    {
        // Last resort: use a default offset with warning
        offset = std::stoi(node->getMetadata("offset"));
        emitComment("Warning: No offset found for " + tempVarName + ", using default");
    }

    // Generate code to load the integer literal
    emitComment("processing: " + tempVarName + " := " + node->getNodeValue());
    emit("addi r" + std::to_string(reg) + ",r0," + std::to_string(value));

    // Store to temporary variable
    emit("sw " + std::to_string(offset) + "(r14),r" + std::to_string(reg));

    // Store register info for parent nodes
    node->setMetadata("reg", std::to_string(reg));
    // Store the offset in the node metadata
    node->setMetadata("offset", std::to_string(offset));

    // Free the register
    freeRegister(reg);
}

void CodeGenVisitor::visitFloat(ASTNode *node)
{
    // TODO: Handle float literals
    // Allocate a register for this float
    int reg = allocateRegister();

    // Get the float value
    float value = std::stof(node->getNodeValue());
    int intValue = static_cast<int>(value);

    // Get metadata that was set by MemSizeVisitor
    std::string tempVarName = node->getMetadata("moonVarName");
    int offset;
    auto symbol = currentTable->lookupSymbol(tempVarName);
    if (symbol && !symbol->getMetadata("offset").empty())
    {
        offset = std::stoi(symbol->getMetadata("offset"));
    }
    else if (!node->getMetadata("offset").empty())
    {
        // Fallback: use offset from node metadata if symbol not found
        offset = std::stoi(node->getMetadata("offset"));
        emitComment("Warning: Symbol for " + tempVarName + " not found, using node metadata offset");
    }
    else
    {
        // Last resort: use a default offset with warning
        offset = std::stoi(node->getMetadata("offset"));
        emitComment("Warning: No offset found for " + tempVarName + ", using default");
    }

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
void CodeGenVisitor::visitAddOp(ASTNode *node)
{
    // Traverse children first
    ASTNode *leftChild = node->getLeftMostChild();
    ASTNode *rightChild = leftChild ? leftChild->getRightSibling() : nullptr;

    if (leftChild)
        leftChild->accept(this);
    if (rightChild)
        rightChild->accept(this);

    // Get registers from child nodes
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    int reg3 = allocateRegister();

    // Get offsets for the operands - handle identifiers separately
    // Handle left operand offset
    int leftOffset;
    if (leftChild->getNodeEnum() == NodeType::IDENTIFIER)
    {
        // For simple identifiers, get offset directly from symbol table
        leftOffset = getSymbolOffset(leftChild->getNodeValue());
    }
    else if (leftChild->getNodeEnum() == NodeType::ARRAY_ACCESS ||
             leftChild->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array access and dot identifier, get from metadata first
        if (!leftChild->getMetadata("offset").empty())
        {
            leftOffset = std::stoi(leftChild->getMetadata("offset"));
        }
        else
        {
            emitComment("Error: Cannot determine offset for complex left expression");
            leftOffset = -8;
        }
    }
    else if (!leftChild->getMetadata("moonVarName").empty())
    {
        // For expressions, lookup the temporary variable in symbol table
        std::string tempVarName = leftChild->getMetadata("moonVarName");
        auto leftSymbol = currentTable->lookupSymbol(tempVarName);
        if (leftSymbol && !leftSymbol->getMetadata("offset").empty())
        {
            leftOffset = std::stoi(leftSymbol->getMetadata("offset"));
        }
        else
        {
            emitComment("Warning: Symbol not found for " + tempVarName);
            leftOffset = (!leftChild->getMetadata("offset").empty()) ? std::stoi(leftChild->getMetadata("offset")) : -8;
        }
    }
    else if (!leftChild->getMetadata("offset").empty())
    {
        // Last resort: use direct metadata
        leftOffset = std::stoi(leftChild->getMetadata("offset"));
    }
    else
    {
        emitComment("Error: Cannot determine left operand offset");
        leftOffset = -8; // Default
    }

    // Handle right operand offset
    int rightOffset;
    if (rightChild->getNodeEnum() == NodeType::IDENTIFIER)
    {
        // For simple identifiers, get offset directly from symbol table
        rightOffset = getSymbolOffset(rightChild->getNodeValue());
    }
    else if (rightChild->getNodeEnum() == NodeType::ARRAY_ACCESS ||
             rightChild->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array access and dot identifier, get from metadata first
        if (!rightChild->getMetadata("offset").empty())
        {
            rightOffset = std::stoi(rightChild->getMetadata("offset"));
        }
        else
        {
            emitComment("Error: Cannot determine offset for complex right expression");
            rightOffset = -8;
        }
    }
    else if (!rightChild->getMetadata("moonVarName").empty())
    {
        // For expressions, lookup the temporary variable in symbol table
        std::string tempVarName = rightChild->getMetadata("moonVarName");
        auto rightSymbol = currentTable->lookupSymbol(tempVarName);
        if (rightSymbol && !rightSymbol->getMetadata("offset").empty())
        {
            rightOffset = std::stoi(rightSymbol->getMetadata("offset"));
        }
        else
        {
            emitComment("Warning: Symbol not found for " + tempVarName);
            rightOffset = (!rightChild->getMetadata("offset").empty()) ? std::stoi(rightChild->getMetadata("offset")) : -8;
        }
    }
    else if (!rightChild->getMetadata("offset").empty())
    {
        // Last resort: use direct metadata
        rightOffset = std::stoi(rightChild->getMetadata("offset"));
    }
    else
    {
        emitComment("Error: Cannot determine right operand offset");
        rightOffset = -8; // Default
    }

    // Get result metadata (already set by MemSizeVisitor)
    int resultOffset;
    std::string resultVarName = node->getMetadata("moonVarName");
    if (!resultVarName.empty())
    {
        auto symbol = currentTable->lookupSymbol(resultVarName);
        if (symbol && !symbol->getMetadata("offset").empty())
        {
            resultOffset = std::stoi(symbol->getMetadata("offset"));
        }
        else if (!node->getMetadata("offset").empty())
        {
            // Fallback: use offset from node metadata if symbol not found
            resultOffset = std::stoi(node->getMetadata("offset"));
            emitComment("Warning: Symbol for " + resultVarName + " not found, using node metadata offset");
        }
        else
        {
            // Last resort: use a default offset with warning
            resultOffset = -8;
            emitComment("Warning: No offset found for " + resultVarName + ", using default");
        }
    }
    else if (!node->getMetadata("offset").empty())
    {
        // Direct offset available in metadata
        resultOffset = std::stoi(node->getMetadata("offset"));
    }
    else
    {
        // No metadata available at all
        resultOffset = -8;
        emitComment("Warning: No offset found for comparison result, using default");
    }

    // Generate code for the operation
    std::string opStr = node->getNodeValue();
    emitComment("processing: " + resultVarName + " := " +
                (leftChild->getNodeEnum() == NodeType::IDENTIFIER ? leftChild->getNodeValue() : leftChild->getMetadata("moonVarName")) + " " + opStr + " " +
                (rightChild->getNodeEnum() == NodeType::IDENTIFIER ? rightChild->getNodeValue() : rightChild->getMetadata("moonVarName")));

    // Load values into registers
    // Get left operand value into register
    if (leftChild->getNodeEnum() == NodeType::ARRAY_ACCESS ||
        leftChild->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array/member access, we need to do a double load:
        // 1. Load the address from the offset
        // 2. Load the value from that address
        emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
        emit("lw r" + std::to_string(reg1) + ",0(r" + std::to_string(reg1) + ")");
    }
    else
    {
        // For simple variables or temporary values
        emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
    }

    // Get right operand value into register
    if (rightChild->getNodeEnum() == NodeType::ARRAY_ACCESS ||
        rightChild->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array/member access, do a double load
        emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
        emit("lw r" + std::to_string(reg2) + ",0(r" + std::to_string(reg2) + ")");
    }
    else
    {
        // For simple variables or temporary values
        emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
    }

    // Perform addition/subtraction/or
    if (opStr == "+")
    {
        emit("add r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (opStr == "-")
    {
        emit("sub r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (opStr == "or")
    {
        // Logical OR implementation
        std::string trueLabel = "logical_or_true_" + std::to_string(labelCounter);
        std::string endLabel = "logical_or_end_" + std::to_string(labelCounter++);

        emit("bnz r" + std::to_string(reg1) + "," + trueLabel); // If left operand is non-zero, result is 1
        emit("bnz r" + std::to_string(reg2) + "," + trueLabel); // If right operand is non-zero, result is 1
        emit("addi r" + std::to_string(reg3) + ",r0,0");        // Both are zero, result is 0
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

void CodeGenVisitor::visitMultOp(ASTNode *node)
{
    // Traverse children first
    ASTNode *leftChild = node->getLeftMostChild();
    ASTNode *rightChild = leftChild ? leftChild->getRightSibling() : nullptr;

    if (leftChild)
        leftChild->accept(this);
    if (rightChild)
        rightChild->accept(this);

    // Get registers
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    int reg3 = allocateRegister();

    // Get operand values into registers using improved offset lookup
    int leftOffset = getNodeOffset(leftChild);
    int rightOffset = getNodeOffset(rightChild);

    // Load left operand into register with special handling for complex types
    if (leftChild->getNodeEnum() == NodeType::ARRAY_ACCESS ||
        leftChild->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array/member access, we need to do a double load:
        // 1. Load the address from the offset
        // 2. Load the value from that address
        emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
        emit("lw r" + std::to_string(reg1) + ",0(r" + std::to_string(reg1) + ")");
    }
    else
    {
        // For simple variables or temporary values
        emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
    }

    // Load right operand into register with special handling for complex types
    if (rightChild->getNodeEnum() == NodeType::ARRAY_ACCESS ||
        rightChild->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array/member access, do a double load
        emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
        emit("lw r" + std::to_string(reg2) + ",0(r" + std::to_string(reg2) + ")");
    }
    else
    {
        // For simple variables or temporary values
        emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
    }

    // Get result metadata (already set by MemSizeVisitor)
    int resultOffset;
    std::string resultVarName = node->getMetadata("moonVarName");
    if (!resultVarName.empty())
    {
        auto symbol = currentTable->lookupSymbol(resultVarName);
        if (symbol && !symbol->getMetadata("offset").empty())
        {
            resultOffset = std::stoi(symbol->getMetadata("offset"));
        }
        else if (!node->getMetadata("offset").empty())
        {
            // Fallback: use offset from node metadata if symbol not found
            resultOffset = std::stoi(node->getMetadata("offset"));
            emitComment("Warning: Symbol for " + resultVarName + " not found, using node metadata offset");
        }
        else
        {
            // Last resort: use a default offset with warning
            resultOffset = -8;
            emitComment("Warning: No offset found for " + resultVarName + ", using default");
        }
    }
    else if (!node->getMetadata("offset").empty())
    {
        // Direct offset available in metadata
        resultOffset = std::stoi(node->getMetadata("offset"));
    }
    else
    {
        // No metadata available at all
        resultOffset = -8;
        emitComment("Warning: No offset found for multiplication result, using default");
    }

    // Generate code for the operation
    std::string opStr = node->getNodeValue();
    emitComment("processing: " + resultVarName + " := " +
                (leftChild->getNodeEnum() == NodeType::IDENTIFIER ? leftChild->getNodeValue() : leftChild->getMetadata("moonVarName")) + " " + opStr + " " +
                (rightChild->getNodeEnum() == NodeType::IDENTIFIER ? rightChild->getNodeValue() : rightChild->getMetadata("moonVarName")));

    // Perform multiplication/division/and
    if (opStr == "*")
    {
        emit("mul r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (opStr == "/")
    {
        emit("div r" + std::to_string(reg3) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (opStr == "and")
    {
        // Logical AND implementation
        std::string zeroLabel = "logical_and_zero_" + std::to_string(labelCounter);
        std::string endLabel = "logical_and_end_" + std::to_string(labelCounter++);

        emit("bz r" + std::to_string(reg1) + "," + zeroLabel); // If left operand is 0, result is 0
        emit("bz r" + std::to_string(reg2) + "," + zeroLabel); // If right operand is 0, result is 0
        emit("addi r" + std::to_string(reg3) + ",r0,1");       // Both are non-zero, result is 1
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

// Implementation for assignment
void CodeGenVisitor::visitAssignment(ASTNode *node)
{
    // Get variable node and expression node
    ASTNode *varNode = node->getLeftMostChild();
    ASTNode *exprNode = varNode ? varNode->getRightSibling() : nullptr;

    if (!varNode || !exprNode)
        return;

    // Process the expression first to get its value
    exprNode->accept(this);

    // Get the value from the expression into a register
    int valueReg = allocateRegister();
    int exprOffset;

    // Determine the offset based on node type and available metadata
    if (exprNode->getNodeEnum() == NodeType::IDENTIFIER)
    {
        // For identifiers, get offset from symbol table
        exprOffset = getSymbolOffset(exprNode->getNodeValue());
    }
    else if (exprNode->getNodeEnum() == NodeType::ARRAY_ACCESS)
    {
        // For array access, we need to get the calculated address from memory
        // The array access node should have visited already and stored its offset
        if (!exprNode->getMetadata("offset").empty())
        {
            // This offset contains the memory location where the array element address is stored
            int arrayAddressOffset = std::stoi(exprNode->getMetadata("offset"));

            // For reads, load from the address
            int addrReg = allocateRegister();
            emit("lw r" + std::to_string(addrReg) + "," + std::to_string(arrayAddressOffset) + "(r14)");
            // emit("add r" + std::to_string(addrReg) + ",r"+std::to_string(addrReg)+",r14");

            // Create a new temporary for the value
            int tempOffset = getScopeOffset(currentTable);
            emit("lw r" + std::to_string(valueReg) + ",0(r" + std::to_string(addrReg) + ")");
            emit("sw " + std::to_string(tempOffset) + "(r14),r" + std::to_string(valueReg));
            freeRegister(addrReg);
            exprOffset = tempOffset;
        }
        else
        {
            emitComment("Error: Array access missing offset metadata");
            exprOffset = -8; // Use default
        }
    }
    else if (!exprNode->getMetadata("offset").empty())
    {
        // For expressions with metadata, use that
        exprOffset = getSymbolOffset(exprNode->getMetadata("moonVarName"));
    }
    else
    {
        // Handle complex expressions by trying to find a symbol
        std::string nodeName = exprNode->getNodeValue();
        if (!nodeName.empty())
        {
            // Try to find the symbol in the current table
            auto symbol = currentTable->lookupSymbol(nodeName);
            if (symbol && !symbol->getMetadata("offset").empty())
            {
                exprOffset = std::stoi(symbol->getMetadata("offset"));
            }
            else
            {
                emitComment("Warning: Could not determine offset for expression");
                exprOffset = -8; // Use a default offset
            }
        }
        else
        {
            emitComment("Warning: Could not determine offset for expression");
            exprOffset = -8; // Use a default offset
        }
    }

    emit("lw r" + std::to_string(valueReg) + "," + std::to_string(exprOffset) + "(r14)");

    // Handle different types of left-side expressions
    if (varNode->getNodeEnum() == NodeType::ARRAY_ACCESS)
    {
        // For array access, we need to calculate the address
        varNode->accept(this);

        // Get the address of the array element
        int addressReg = allocateRegister();
        int addrOffset = std::stoi(varNode->getMetadata("offset"));

        // Store the value to the calculated address
        emitComment("Storing value into array element");
        emit("add r" + std::to_string(addressReg) + ",r0,r0");
        emit("lw r" + std::to_string(addressReg) + "," + std::to_string(addrOffset) + "(r14)");
        // emit("add r" + std::to_string(addressReg) + ",r"+ std::to_string(addressReg) +",r14");
        emit("sw 0(r" + std::to_string(addressReg) + "),r" + std::to_string(valueReg));

        freeRegister(addressReg);
    }
    else if (varNode->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // Handle class member access (obj.field)
        varNode->accept(this);

        // Get the calculated member address
        int addressReg = allocateRegister();
        int addrOffset = std::stoi(varNode->getMetadata("offset"));
        emit("lw r" + std::to_string(addressReg) + "," + std::to_string(addrOffset) + "(r14)");

        // Store the value to the member address
        emitComment("Storing value into class member");
        emit("sw 0(r" + std::to_string(addressReg) + "),r" + std::to_string(valueReg));

        freeRegister(addressReg);
    }
    else
    {
        // Simple variable assignment
        std::string varName = varNode->getNodeValue();
        int varOffset = getSymbolOffset(varName);

        // Generate assignment code
        emitComment("processing:: " + varName + " := " + exprNode->getMetadata("moonVarName"));
        emit("sw " + std::to_string(varOffset) + "(r14),r" + std::to_string(valueReg));
    }
    emitComment("Assignment completed");
    // Free the register
    freeRegister(valueReg);
}

void CodeGenVisitor::visitArrayAccess(ASTNode *node)
{
    emitComment("Processing array access");

    ASTNode *arrayBaseNode = node->getLeftMostChild();
    ASTNode *indexListNode = arrayBaseNode ? arrayBaseNode->getRightSibling() : nullptr;

    if (!arrayBaseNode || !indexListNode || indexListNode->getNodeEnum() != NodeType::INDEX_LIST)
    {
        emitComment("Error: Invalid ArrayAccess structure");
        return;
    }

    // --- Get Array Base Address ---
    int baseAddrReg = allocateRegister();
    if (arrayBaseNode->getNodeEnum() == NodeType::IDENTIFIER)
    {
        // Array base address calculation for simple variables
        std::string arrayName = arrayBaseNode->getNodeValue();
        auto arraySymbol = currentTable->lookupSymbol(arrayName);
        if (!arraySymbol)
        {
            emitComment("Error: Array symbol not found: " + arrayName);
            freeRegister(baseAddrReg);
            return;
        }
        int arrayMemOffset = getSymbolOffset(arrayName);
        emit("subi r" + std::to_string(baseAddrReg) + ",r14," + std::to_string(arrayMemOffset * -1));
    }
    else if (arrayBaseNode->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // Array base address calculation for class members
        arrayBaseNode->accept(this);
        std::string memberAddrLocStr = arrayBaseNode->getMetadata("address_location");
        if (memberAddrLocStr.empty())
        {
            emitComment("Error: Dot identifier did not provide address location");
            freeRegister(baseAddrReg);
            return;
        }
        emit("lw r" + std::to_string(baseAddrReg) + "," + memberAddrLocStr + "(r14)");
    }
    else
    {
        emitComment("Error: Unsupported array base type");
        freeRegister(baseAddrReg);
        return;
    }

    // --- Calculate Byte Offset via Index List ---
    int totalOffsetReg = allocateRegister();

    // Pass the register to IndexList via metadata
    indexListNode->setMetadata("total_offset_reg", std::to_string(totalOffsetReg));
    indexListNode->accept(this);
    std::string byteOffsetLocStr = indexListNode->getMetadata("byte_offset_loc");
    if (byteOffsetLocStr.empty())
    {
        emitComment("Error: IndexList did not calculate byte offset location");
        freeRegister(baseAddrReg);
        return;
    }
    emitComment("Array byte offset calculated");
    int byteOffsetLoc = std::stoi(byteOffsetLocStr);

    // --- Calculate Final Address ---
    int finalAddrReg = allocateRegister();
    emit("sub r" + std::to_string(finalAddrReg) + ",r" + std::to_string(baseAddrReg) + ",r" + std::to_string(totalOffsetReg));

    int finalAddrOffsetTemp;
    if (indexListNode->getMetadata("moonVarName").empty())
    {
        emitComment("Error: No offset metadata for array access");
        freeRegister(baseAddrReg);
        freeRegister(totalOffsetReg);
        freeRegister(finalAddrReg);
        return;
    }
    else
    {
        auto symbol = currentTable->lookupSymbol(indexListNode->getMetadata("moonVarName"));
        if (symbol)
        {
            finalAddrOffsetTemp = getSymbolOffset(symbol->getName());
        }
        else
        {
            std::cerr << "Error: Symbol not found for " << indexListNode->getMetadata("moonVarName") << std::endl;
            finalAddrOffsetTemp = -1; // Default or error value
        }
    }

    emit("sw " + std::to_string(finalAddrOffsetTemp) + "(r14),r" + std::to_string(finalAddrReg));
    node->setMetadata("offset", std::to_string(finalAddrOffsetTemp));
    emitComment("Array access address stored in " + std::to_string(finalAddrOffsetTemp) + "(r14)");
    // Free temporary registers
    freeRegister(totalOffsetReg);
    freeRegister(baseAddrReg);
    freeRegister(finalAddrReg);
}

void CodeGenVisitor::visitIndexList(ASTNode *node)
{
    emitComment("Calculating array byte offset");

    // Get the parent ArrayAccess node to access array info
    ASTNode *arrayAccessNode = node->getParent();
    if (!arrayAccessNode || arrayAccessNode->getNodeEnum() != NodeType::ARRAY_ACCESS)
    {
        emitComment("Error: IndexList parent is not ArrayAccess");
        return;
    }

    // Get array base node to find symbol and dimensions
    ASTNode *arrayBaseNode = arrayAccessNode->getLeftMostChild();
    if (!arrayBaseNode)
    {
        emitComment("Error: ArrayAccess node missing base array child");
        return;
    }

    // Retrieve dimensions and element size from the array symbol
    std::vector<int> dimensions;
    int elementSize = 4; // Default to int size (4 bytes)
    std::shared_ptr<Symbol> arraySymbol = nullptr;

    // --- Get Array Symbol and Type Info ---
    // We need the symbol to know the dimensions and element size.
    // This part might need adjustment if the base is a complex expression or dot_identifier
    // For now, assume it's primarily an IDENTIFIER.
    if (arrayBaseNode->getNodeEnum() == NodeType::IDENTIFIER)
    {
        arraySymbol = currentTable->lookupSymbol(arrayBaseNode->getNodeValue());
        if (arraySymbol)
        {
            dimensions = arraySymbol->getArrayDimensions();
            std::string baseType = arraySymbol->getType();
            // Basic type extraction (remove brackets)
            size_t bracketPos = baseType.find('[');
            if (bracketPos != std::string::npos)
            {
                baseType = baseType.substr(0, bracketPos);
            }
            // TODO: Add proper size lookup based on type (e.g., float = 8)
            if (baseType == "float")
            {
                elementSize = 8; // Assuming float size is 8
            }
        }
        else
        {
            emitComment("Error: Array symbol not found for " + arrayBaseNode->getNodeValue());
            return;
        }
    }
    else
    {
        // TODO: Handle DOT_IDENTIFIER or other complex base expressions
        // Need a way to get type info (dimensions, element size) propagated
        // For now, assume int[?]... with size 4
        emitComment("Warning: Assuming int array for non-identifier base");
        // We might need dimensions from type checking phase metadata if symbol isn't directly available
    }

    if (dimensions.empty())
    {
        emitComment("Error: Cannot calculate offset for non-array or unknown dimensions");
        return;
    }
    // --- End Get Array Symbol ---

    int totalOffsetReg;
    if (!node->getMetadata("total_offset_reg").empty())
    {
        totalOffsetReg = std::stoi(node->getMetadata("total_offset_reg"));
    }
    else
    {
        emitComment("Error: No offset register provided by ArrayAccess");
        return; // Cannot proceed without proper register management
    }
    emit("addi r" + std::to_string(totalOffsetReg) + ",r0,0"); // Initialize total byte offset = 0

    ASTNode *indexNode = node->getLeftMostChild();
    int dimIndex = 0; // Keep track of which dimension we are processing

    while (indexNode && dimIndex < dimensions.size())
    {
        // First, visit the index node to evaluate any expressions
        indexNode->accept(this);

        // Now get the offset of the temporary variable holding the index value
        int indexValueOffset = -1; // Default to invalid offset

        // For simple identifiers, just use the variable name directly
        if (indexNode->getNodeEnum() == NodeType::IDENTIFIER)
        {
            indexValueOffset = getSymbolOffset(indexNode->getNodeValue());
        }
        // For temporary variables, first get the moonVarName and then look it up
        else if (!indexNode->getMetadata("moonVarName").empty())
        {
            std::string tempVarName = indexNode->getMetadata("moonVarName");
            indexValueOffset = getSymbolOffset(tempVarName);

            // If lookup failed, emit warning
            if (indexValueOffset == 0)
            {
                emitComment("Warning: Temp var " + tempVarName + " not found in symbol table");
            }
        }
        // Last resort: use direct offset metadata if available
        else if (!indexNode->getMetadata("offset").empty())
        {
            indexValueOffset = std::stoi(indexNode->getMetadata("offset"));
        }
        // If all methods fail, issue error and skip
        else
        {
            emitComment("Error: Cannot determine index value location for " +
                        (indexNode->getNodeValue().empty() ? "expression" : indexNode->getNodeValue()));
            indexNode = indexNode->getRightSibling();
            dimIndex++;
            continue;
        }

        // 2. Load the index value from its temporary variable
        int indexValueReg = allocateRegister();
        emit("lw r" + std::to_string(indexValueReg) + "," + std::to_string(indexValueOffset) + "(r14)");

        // 3. Calculate the size multiplier for this dimension
        // multiplier = elementSize * product_of_sizes_of_subsequent_dimensions
        int sizeMultiplier = elementSize;
        for (size_t k = 0; k < dimensions.size() - dimIndex - 1; ++k)
        {
            if (dimensions[k] > 0)
            { // Ignore dynamic dimensions for static offset calc
                sizeMultiplier *= dimensions[k];
            }
            else
            {
                emitComment("Warning: Dynamic dimension encountered, offset calculation might be inaccurate");
                // Handle dynamic arrays if needed - requires runtime info
            }
        }

        // 4. Calculate the byte offset contribution for this index
        int contributionReg = allocateRegister();
        emit("muli r" + std::to_string(contributionReg) + ",r" + std::to_string(indexValueReg) + "," + std::to_string(sizeMultiplier));

        // 5. Add contribution to the total offset
        emit("add r" + std::to_string(totalOffsetReg) + ",r" + std::to_string(totalOffsetReg) + ",r" + std::to_string(contributionReg));

        // Free temporary registers for this index
        freeRegister(indexValueReg);
        freeRegister(contributionReg);

        // Move to the next index/dimension
        indexNode = indexNode->getRightSibling();
        dimIndex++;
    }

    // Check if the number of indices matched the dimensions used for calculation
    if (dimIndex != dimensions.size() || indexNode != nullptr)
    {
        // This check might be better placed in semantic analysis
        // emitComment("Warning: Number of indices might not match array dimensions");
    }

    // Store the final calculated byte offset into a new temporary variable
    int finalByteOffsetLoc = getScopeOffset(currentTable); // Get next available stack offset
    // emit("sw " + std::to_string(finalByteOffsetLoc) + "(r14),r" + std::to_string(totalOffsetReg));

    // Store the *location* of the final byte offset in this node's metadata
    node->setMetadata("byte_offset_loc", std::to_string(finalByteOffsetLoc));
    node->setMetadata("offset_reg", std::to_string(totalOffsetReg));
}

// Example default visitor method for nodes that just traverse children
void CodeGenVisitor::visitEmpty(ASTNode *node)
{
    ASTNode *child = node->getLeftMostChild();
    while (child)
    {
        child->accept(this);
        child = child->getRightSibling();
    }
}

// Default implementation for most visitor methods
#define DEFAULT_VISITOR_METHOD(method)                \
    void CodeGenVisitor::visit##method(ASTNode *node) \
    {                                                 \
        ASTNode *child = node->getLeftMostChild();    \
        while (child)                                 \
        {                                             \
            child->accept(this);                      \
            child = child->getRightSibling();         \
        }                                             \
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
// DEFAULT_VISITOR_METHOD(ArrayAccess)
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
DEFAULT_VISITOR_METHOD(DimList)

void CodeGenVisitor::visitFunction(ASTNode *node)
{
    // Get function signature (first child)
    ASTNode *functionSignature = node->getLeftMostChild();
    if (!functionSignature)
        return;

    // Get function ID from inside the signature
    ASTNode *functionId = functionSignature->getLeftMostChild();
    if (!functionId)
        return;

    std::string functionName = functionId->getNodeValue();
    currentFunction = functionName;

    codeSection += "\n";
    emitComment("Processing function: " + functionName);

    // Save previous table before changing scope
    std::shared_ptr<SymbolTable> previousTable = currentTable;

    // Find and set the function's symbol table as current
    // Fix the table lookup to use the correct naming convention with parameter types
    auto funcSymbols = symbolTable->lookupFunctions(functionName);
    std::shared_ptr<SymbolTable> nestedTable = nullptr;

    if (!funcSymbols.empty())
    {
        // Format the unique key using the same convention as in SymbolTableVisitor
        std::string uniqueKey = functionName;
        const auto &paramTypes = funcSymbols[0]->getParams();

        if (!paramTypes.empty())
        {
            uniqueKey += "_";
            for (size_t i = 0; i < paramTypes.size(); ++i)
            {
                // Replace any special characters that might cause issues in table names
                std::string cleanType = paramTypes[i];
                std::replace(cleanType.begin(), cleanType.end(), '[', '_');
                std::replace(cleanType.begin(), cleanType.end(), ']', '_');
                std::replace(cleanType.begin(), cleanType.end(), ' ', '_');

                uniqueKey += cleanType;
                if (i < paramTypes.size() - 1)
                {
                    uniqueKey += "_";
                }
            }
        }

        // Now try to get the table with the formatted key
        nestedTable = symbolTable->getNestedTable(uniqueKey);

        if (!nestedTable)
        {
            // Fallback: try with just the function name (for simple cases)
            nestedTable = symbolTable->getNestedTable(functionName);
        }
    }
    else
    {
        // Fallback to just the function name if no symbols found
        nestedTable = symbolTable->getNestedTable(functionName);
    }

    if (nestedTable)
    {
        currentTable = nestedTable;
        emitComment("Entering function scope: " + functionName);
    }

    // Special handling for main function (entry point)
    if (functionName == "main")
    {
        emitLabel(functionName + "      entry");
        emit("addi r14,r0,topaddr");
    }
    else
    {
        // For non-main functions, generate prologue
        generateFunctionPrologue(functionName);
    }

    // Process function signature (parameters, etc.)
    functionSignature->accept(this);

    // Process function body (right sibling of signature)
    ASTNode *functionBody = functionSignature->getRightSibling();
    if (functionBody)
    {
        functionBody->accept(this);
    }

    // Generate function epilogue for non-main functions
    if (functionName != "main")
    {
        generateFunctionEpilogue();
    }

    // Restore previous table when leaving function scope
    currentTable = previousTable;
}

void CodeGenVisitor::visitWriteStatement(ASTNode *node)
{
    // Get the expression to print
    ASTNode *exprNode = node->getLeftMostChild();
    if (!exprNode)
        return;

    // Process the expression first
    exprNode->accept(this);

    // Allocate registers
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();

    // Get the output expression offset - handle identifiers correctly
    int exprOffset = getNodeOffset(exprNode);
    // Determine the offset based on node type and available metadata
    if (exprNode->getNodeEnum() == NodeType::ARRAY_ACCESS)
    {
        // For array access, we need to get the calculated address from memory
        // The array access node should have visited already and stored its offset
        if (!exprNode->getMetadata("offset").empty())
        {
            // This offset contains the memory location where the array element address is stored
            int arrayAddressOffset = exprOffset;

            // For reads, load from the address
            int addrReg = allocateRegister();
            emit("lw r" + std::to_string(addrReg) + "," + std::to_string(arrayAddressOffset) + "(r14)");
            // emit("add r" + std::to_string(addrReg) + ",r"+std::to_string(addrReg)+",r14");

            // Create a new temporary for the value
            // int tempOffset = getScopeOffset(currentTable);
            emit("lw r" + std::to_string(reg1) + ",0(r" + std::to_string(addrReg) + ")");
            freeRegister(addrReg);
            // exprOffset = tempOffset;
        }
        else
        {
            emitComment("Error: Array access missing offset metadata");
            exprOffset = -8; // Use default
        }
    }
    else if (exprNode->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For object member access, similar to array access
        if (!exprNode->getMetadata("offset").empty())
        {
            int memberAddressOffset = std::stoi(exprNode->getMetadata("offset"));

            // Determine if this is a read operation
            ASTNode *parentNode = exprNode->getParent();
            bool isAddressOnly = (parentNode && parentNode->getNodeEnum() == NodeType::ASSIGNMENT &&
                                  parentNode->getLeftMostChild() == exprNode);

            if (!isAddressOnly)
            {
                // For reads, load from the member address
                int addrReg = allocateRegister();
                emit("lw r" + std::to_string(addrReg) + "," + std::to_string(memberAddressOffset) + "(r14)");

                // Create a new temporary for the value
                int tempOffset = getScopeOffset(currentTable);
                emit("lw r" + std::to_string(addrReg) + ",0(r" + std::to_string(addrReg) + ")");
                emit("sw " + std::to_string(tempOffset) + "(r14),r" + std::to_string(addrReg));
                freeRegister(addrReg);

                exprOffset = tempOffset;
            }
            else
            {
                // For writes, just use the stored address
                exprOffset = memberAddressOffset;
            }
        }
        else
        {
            emitComment("Error: Member access missing offset metadata");
            exprOffset = -8; // Use default
        }
    }

    int scopeOffset = getScopeOffset(currentTable);

    // Get the variable name for the comment
    std::string varName = (exprNode->getNodeEnum() == NodeType::IDENTIFIER) ? exprNode->getNodeValue() : exprNode->getMetadata("moonVarName");

    emitComment("processing: write(" + varName + ")");
    if (exprNode->getNodeEnum() != NodeType::DOT_IDENTIFIER && exprNode->getNodeEnum() != NodeType::ARRAY_ACCESS)
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
    emit("jl r15,putnl"); // add newline
    emit("subi r14,r14," + std::to_string(scopeOffset));

    // Free registers
    freeRegister(reg1);
    freeRegister(reg2);
}

void CodeGenVisitor::visitReadStatement(ASTNode *node)
{
    // Get the variable to read into
    ASTNode *varNode = node->getLeftMostChild();
    if (!varNode)
        return;

    // Allocate registers
    int reg1 = allocateRegister();

    // Get the variable offset - handle identifiers
    int varOffset;
    if (varNode->getNodeEnum() == NodeType::IDENTIFIER)
    {
        // For identifiers, get offset from symbol table
        varOffset = getSymbolOffset(varNode->getNodeValue());
    }
    else
    {
        // For expressions, get from metadata
        varOffset = std::stoi(varNode->getMetadata("offset"));
    }

    int scopeOffset = getScopeOffset(currentTable);

    // Get the variable name for the comment
    std::string varName = (varNode->getNodeEnum() == NodeType::IDENTIFIER) ? varNode->getNodeValue() : varNode->getMetadata("moonVarName");

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

void CodeGenVisitor::visitIfStatement(ASTNode *node)
{
    // Create unique labels for branching
    std::string thenLabel = "then_" + std::to_string(labelCounter++);
    std::string elseLabel = "else_" + std::to_string(labelCounter++);
    std::string endifLabel = "endif_" + std::to_string(labelCounter++);

    // Get the three children: condition, then-block, and else-block
    ASTNode *conditionNode = node->getLeftMostChild();
    ASTNode *thenBlock = conditionNode ? conditionNode->getRightSibling() : nullptr;
    ASTNode *elseBlock = thenBlock ? thenBlock->getRightSibling() : nullptr;

    emitComment("if statement - evaluating condition");

    // Process condition and get its result
    if (conditionNode)
    {
        conditionNode->accept(this);

        // Get the register or memory location holding the condition result
        int condReg = allocateRegister();
        int condOffset;

        if (conditionNode->getMetadata("reg").empty())
        {
            // Condition result is in memory, load it
            condOffset = std::stoi(conditionNode->getMetadata("offset"));
            emit("lw r" + std::to_string(condReg) + "," + std::to_string(condOffset) + "(r14)");
        }
        else
        {
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
    if (thenBlock)
    {
        thenBlock->accept(this);
    }
    emit("j " + endifLabel);

    // Else block
    emitComment("else branch");
    emitLabel(elseLabel);
    if (elseBlock)
    {
        elseBlock->accept(this);
    }

    // End of if statement
    emitLabel(endifLabel);
    emitComment("end if");
}

void CodeGenVisitor::visitCondition(ASTNode *node)
{
    // A condition has a rel_op node as its only child
    ASTNode *relOpNode = node->getLeftMostChild();
    if (!relOpNode)
        return;

    // Process the rel_op node
    relOpNode->accept(this);

    // Pass register info up to parent node
    if (!relOpNode->getMetadata("reg").empty())
    {
        node->setMetadata("reg", relOpNode->getMetadata("reg"));
    }

    // Pass memory location info up to parent node
    if (!relOpNode->getMetadata("offset").empty())
    {
        node->setMetadata("offset", relOpNode->getMetadata("offset"));
    }
}

void CodeGenVisitor::visitRelOp(ASTNode *node)
{
    // Get the left and right arithmetic expressions
    ASTNode *leftExpr = node->getLeftMostChild();
    ASTNode *rightExpr = leftExpr ? leftExpr->getRightSibling() : nullptr;

    if (!leftExpr || !rightExpr)
        return;

    // Process operands
    leftExpr->accept(this);
    rightExpr->accept(this);

    // Allocate registers for the comparison
    int reg1 = allocateRegister();
    int reg2 = allocateRegister();
    int resultReg = allocateRegister();

    // Get operand values into registers using improved offset lookup
    int leftOffset = getNodeOffset(leftExpr);
    int rightOffset = getNodeOffset(rightExpr);

    // Load left operand into register with special handling for complex types
    if (leftExpr->getNodeEnum() == NodeType::ARRAY_ACCESS ||
        leftExpr->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array/member access, we need to do a double load:
        // 1. Load the address from the offset
        // 2. Load the value from that address
        emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
        emit("lw r" + std::to_string(reg1) + ",0(r" + std::to_string(reg1) + ")");
    }
    else
    {
        // For simple variables or temporary values
        emit("lw r" + std::to_string(reg1) + "," + std::to_string(leftOffset) + "(r14)");
    }

    // Load right operand into register with special handling for complex types
    if (rightExpr->getNodeEnum() == NodeType::ARRAY_ACCESS ||
        rightExpr->getNodeEnum() == NodeType::DOT_IDENTIFIER)
    {
        // For array/member access, do a double load
        emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
        emit("lw r" + std::to_string(reg2) + ",0(r" + std::to_string(reg2) + ")");
    }
    else
    {
        // For simple variables or temporary values
        emit("lw r" + std::to_string(reg2) + "," + std::to_string(rightOffset) + "(r14)");
    }

    // Get the relational operator
    std::string op = node->getNodeValue();

    // Use Moon's comparison instructions
    if (op == ">")
    {
        emit("cgt r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (op == ">=")
    {
        emit("cge r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (op == "<")
    {
        emit("clt r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (op == "<=")
    {
        emit("cle r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (op == "==")
    {
        emit("ceq r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }
    else if (op == "<>")
    {
        emit("cne r" + std::to_string(resultReg) + ",r" + std::to_string(reg1) + ",r" + std::to_string(reg2));
    }

    // Store the result using the proper offset from symbol table
    int resultOffset;
    std::string resultVarName = node->getMetadata("moonVarName");
    if (!resultVarName.empty())
    {
        auto symbol = currentTable->lookupSymbol(resultVarName);
        if (symbol && !symbol->getMetadata("offset").empty())
        {
            resultOffset = std::stoi(symbol->getMetadata("offset"));
        }
        else if (!node->getMetadata("offset").empty())
        {
            resultOffset = std::stoi(node->getMetadata("offset"));
            emitComment("Warning: Symbol for " + resultVarName + " not found, using node metadata offset");
        }
        else
        {
            resultOffset = -8;
            emitComment("Warning: No offset found for comparison result, using default");
        }
    }
    else if (!node->getMetadata("offset").empty())
    {
        resultOffset = std::stoi(node->getMetadata("offset"));
    }
    else
    {
        resultOffset = -8;
        emitComment("Warning: No offset found for comparison result, using default");
    }

    emit("sw " + std::to_string(resultOffset) + "(r14),r" + std::to_string(resultReg));
    node->setMetadata("reg", std::to_string(resultReg));

    // Free registers for operands
    freeRegister(reg1);
    freeRegister(reg2);
}

void CodeGenVisitor::visitWhileStatement(ASTNode *node)
{
    // Create unique labels for the loop
    std::string whileStartLabel = "while_start_" + std::to_string(labelCounter++);
    std::string whileEndLabel = "while_end_" + std::to_string(labelCounter++);

    // Get the condition and body blocks
    ASTNode *conditionNode = node->getLeftMostChild();
    ASTNode *bodyNode = conditionNode ? conditionNode->getRightSibling() : nullptr;

    // Label for the start of the loop (condition evaluation)
    emitLabel(whileStartLabel);
    emitComment("while loop - evaluating condition");

    // Process the condition
    if (conditionNode)
    {
        conditionNode->accept(this);

        // Get the register or memory location holding the condition result
        int condReg = allocateRegister();
        int condOffset;

        if (conditionNode->getMetadata("reg").empty())
        {
            // Condition result is in memory, load it
            condOffset = std::stoi(conditionNode->getMetadata("offset"));
            emit("lw r" + std::to_string(condReg) + "," + std::to_string(condOffset) + "(r14)");
        }
        else
        {
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
    if (bodyNode)
    {
        bodyNode->accept(this);
    }

    // Jump back to the start of the loop
    emit("j " + whileStartLabel);

    // Label for the end of the loop
    emitLabel(whileEndLabel);
    emitComment("end while loop");
}

void CodeGenVisitor::visitFunctionCall(ASTNode *node) {
    // Get function name and parameter list
    ASTNode *idNode = node->getLeftMostChild();
    ASTNode *paramListNode = idNode ? idNode->getRightSibling() : nullptr;

    if (!idNode)
        return;

    std::string functionName = idNode->getNodeValue();

    // Generate comment for function call
    emitComment("Function call: " + functionName);

    // Save current scope offset for restoring later
    int currentScopeOffset = getScopeOffset(currentTable);

    // Look up the function symbol and its nested table
    auto funcSymbols = symbolTable->lookupFunctions(functionName);
    std::shared_ptr<SymbolTable> functionTable = nullptr;
    
    if (!funcSymbols.empty()) {
        // Format the unique key using the same convention as in SymbolTableVisitor
        std::string uniqueKey = functionName;
        const auto &paramTypes = funcSymbols[0]->getParams();

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

        // Try to get the table with the formatted key
        functionTable = symbolTable->getNestedTable(uniqueKey);

        if (!functionTable) {
            // Fallback: try with just the function name (for simple cases)
            functionTable = symbolTable->getNestedTable(functionName);
        }
    }

    // Step 1: Process parameters in reverse order (right to left)
    std::vector<ASTNode *> params;
    if (paramListNode) {
        ASTNode *paramNode = paramListNode->getLeftMostChild();
        while (paramNode) {
            params.push_back(paramNode);
            paramNode = paramNode->getRightSibling();
        }
    }
    // Get parameter names from function symbol for proper offset lookup
    std::vector<std::string> paramNames;
    if (funcSymbols.size() > 0 && functionTable) {
        // Go through all symbols in the function table
        std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols = functionTable->getSymbols();
        for (const auto& pair : symbols) {
            // Each element in the map is a key-value pair where:
            // pair.first is the string key (name)
            // pair.second is the shared_ptr<Symbol>
            std::shared_ptr<Symbol> symbolPtr = pair.second;
            
            // Check if the symbol is a parameter
            if (symbolPtr->getKind() == SymbolKind::PARAMETER) {
                paramNames.push_back(symbolPtr->getName());
            }
        }
        
        // Sort parameters by their offsets to ensure correct order
        std::sort(paramNames.begin(), paramNames.end(), 
            [&functionTable](const std::string& a, const std::string& b) {
                auto symbolA = functionTable->lookupSymbol(a);
                auto symbolB = functionTable->lookupSymbol(b);
                int offsetA = std::stoi(symbolA->getMetadata("offset"));
                int offsetB = std::stoi(symbolB->getMetadata("offset"));
                return offsetA > offsetB; // Sort by ascending offset
            });
            
        emitComment("Found " + std::to_string(paramNames.size()) + " parameters for function " + functionName);
    }
    // Step 2: Evaluate parameters and push them onto the stack
    emitComment("Pushing parameters onto stack");
    int paramPosition = 0;
    // Process parameters in reverse order
    for (size_t i = 0; i < params.size(); ++i) {
        ASTNode* param = params[params.size() - 1 - i];  // Reverse order
        paramPosition = getScopeOffset(currentTable);
        // Visit the parameter to evaluate it
        param->accept(this);
        
        // Get the parameter offset
        int paramOffset = std::stoi(functionTable->lookupSymbol(paramNames[i])->getMetadata("offset"));
        // Restore the scope offset after parameter evaluation
        //setScopeOffset(currentTable, paramScopeOffset);
        paramPosition += paramOffset; 
        // Get parameter value
        int paramReg = allocateRegister();
        int offset = getNodeOffset(param);
        
        // Load parameter value with special handling for complex types
        if (param->getNodeEnum() == NodeType::ARRAY_ACCESS || 
            param->getNodeEnum() == NodeType::DOT_IDENTIFIER) {
            emit("lw r" + std::to_string(paramReg) + "," + std::to_string(offset) + "(r14)");
            emit("lw r" + std::to_string(paramReg) + ",0(r" + std::to_string(paramReg) + ")");
        } else {
            emit("lw r" + std::to_string(paramReg) + "," + std::to_string(offset) + "(r14)");
        }
        
        
        // Push parameter onto the stack at the correct position
        emit("sw " + std::to_string(paramPosition) + "(r14),r" + std::to_string(paramReg));
        
        freeRegister(paramReg);
    }

    // Rest of the function call handling remains the same
    int frameAdjustment = currentScopeOffset;
    
    // Adjust frame pointer for function call
    emitComment("Setting up activation record for function call");
    emit("addi r14,r14," + std::to_string(frameAdjustment));

    // Jump to function with link
    emit("jl r15," + functionName);

    // Restore stack frame after function returns
    emitComment("Function " + functionName + " returned, restoring stack");
    emit("subi r14,r14," + std::to_string(frameAdjustment));

    // Make sure the scope offset is properly restored
    setScopeOffset(currentTable, currentScopeOffset);
}

void CodeGenVisitor::visitReturnStatement(ASTNode *node)
{
    // Get the return value expression, if any
    ASTNode *exprNode = node->getLeftMostChild();
    if (exprNode)
    {
        // Process the expression to evaluate it
        exprNode->accept(this);

        // Get current function
        std::string funcName = currentFunction;
        auto funcSymbols = symbolTable->lookupFunctions(funcName);
        auto funcSymbol = funcSymbols.empty() ? nullptr : funcSymbols[0];

        // Get return value offset
        int returnOffset = -8; // Default if metadata not found
        if (funcSymbol && !funcSymbol->getMetadata("return_offset").empty())
        {
            returnOffset = std::stoi(funcSymbol->getMetadata("return_offset"));
        }

        // Load return value into register
        int retReg = allocateRegister();

        // Get expression offset using the standard helper method
        int exprOffset = getNodeOffset(exprNode);

        // Handle different types of return expressions with special handling for complex types
        if (exprNode->getNodeEnum() == NodeType::ARRAY_ACCESS ||
            exprNode->getNodeEnum() == NodeType::DOT_IDENTIFIER)
        {
            // For array/member access, we need to do a double load:
            // 1. Load the address from the offset
            // 2. Load the value from that address
            emit("lw r" + std::to_string(retReg) + "," + std::to_string(exprOffset) + "(r14)");
            emit("lw r" + std::to_string(retReg) + ",0(r" + std::to_string(retReg) + ")");
        }
        else
        {
            // For simple variables or temporary values
            emit("lw r" + std::to_string(retReg) + "," + std::to_string(exprOffset) + "(r14)");
        }

        // Store the value in the designated return area
        emit("sw 0(r14),r" + std::to_string(retReg));

        // Also put the return value in r13 by convention
        emit("add r13,r" + std::to_string(retReg) + ",r0");

        freeRegister(retReg);
    }

    // Get the link register offset
    std::string funcName = currentFunction;
    auto funcSymbols = symbolTable->lookupFunctions(funcName);
    auto funcSymbol = funcSymbols.empty() ? nullptr : funcSymbols[0];
    int linkOffset = -4; // Default

    if (funcSymbol && !funcSymbol->getMetadata("link_register_offset").empty())
    {
        linkOffset = std::stoi(funcSymbol->getMetadata("link_register_offset"));
    }

    // Restore return address and return
    emit("lw r15," + std::to_string(linkOffset) + "(r14)");
    emit("jr r15");
}

int CodeGenVisitor::getNodeOffset(ASTNode *node)
{
    // First try to get moonVarName and look it up in the symbol table
    if (!node->getMetadata("moonVarName").empty())
    {
        std::string varName = node->getMetadata("moonVarName");
        auto symbol = currentTable->lookupSymbol(varName);
        if (symbol && !symbol->getMetadata("offset").empty())
        {
            return std::stoi(symbol->getMetadata("offset"));
        }
        else
        {
            emitComment("Warning: Symbol not found for " + varName);
        }
    }

    // For direct identifiers, look up in the symbol table
    if (node->getNodeEnum() == NodeType::IDENTIFIER)
    {
        return getSymbolOffset(node->getNodeValue());
    }

    // Fall back to direct offset metadata if available
    if (!node->getMetadata("offset").empty())
    {
        emitComment("Warning: Using direct offset metadata");
        return std::stoi(node->getMetadata("offset"));
    }

    // Last resort
    emitComment("Error: Could not determine offset for node");
    return -8; // Default temporary location
}