#ifndef CODE_GEN_VISITOR_H
#define CODE_GEN_VISITOR_H

#include "Semantics/Visitor.h"
#include "Semantics/SymbolTableVisitor.h"
#include "CodeGenerator/MemSizeVisitor.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <stack>
#include <fstream>

class CodeGenVisitor : public Visitor {
public:
    // Constructor takes symbol table from MemSizeVisitor
    CodeGenVisitor(std::shared_ptr<SymbolTable> symbolTable);
    ~CodeGenVisitor();

    // Process AST to generate code
    void processAST(ASTNode* root);
    
    // Output the generated code to a file
    void generateOutputFile(const std::string& filename);

    // Visitor pattern implementation - all the required methods from Visitor
    void visitProgram(ASTNode* node) override;
    void visitClassList(ASTNode* node) override;
    void visitFunctionList(ASTNode* node) override;
    void visitImplementationList(ASTNode* node) override;
    void visitClass(ASTNode* node) override;
    void visitFunction(ASTNode* node) override;
    void visitImplementation(ASTNode* node) override;
    void visitClassId(ASTNode* node) override;
    void visitInheritanceList(ASTNode* node) override;
    void visitInheritanceId(ASTNode* node) override;
    void visitVisibility(ASTNode* node) override;
    void visitMemberList(ASTNode* node) override;
    void visitMember(ASTNode* node) override;
    void visitVariable(ASTNode* node) override;
    void visitFunctionId(ASTNode* node) override;
    void visitFunctionSignature(ASTNode* node) override;
    void visitFunctionBody(ASTNode* node) override;
    void visitConstructorSignature(ASTNode* node) override;
    void visitLocalVariable(ASTNode* node) override;
    void visitBlock(ASTNode* node) override;
    void visitIfStatement(ASTNode* node) override;
    void visitWhileStatement(ASTNode* node) override;
    void visitRelationalExpr(ASTNode* node) override;
    void visitAssignment(ASTNode* node) override;
    void visitFunctionDeclaration(ASTNode* node) override;
    void visitAttribute(ASTNode* node) override;
    void visitSingleStatement(ASTNode* node) override;
    void visitExpressionStatement(ASTNode* node) override;
    void visitReadStatement(ASTNode* node) override;
    void visitWriteStatement(ASTNode* node) override;
    void visitReturnStatement(ASTNode* node) override;
    void visitAssignOp(ASTNode* node) override;
    void visitRelOp(ASTNode* node) override;
    void visitAddOp(ASTNode* node) override;
    void visitMultOp(ASTNode* node) override;
    void visitIdentifier(ASTNode* node) override;
    void visitSelfIdentifier(ASTNode* node) override;
    void visitType(ASTNode* node) override;
    void visitArrayDimension(ASTNode* node) override;
    void visitParam(ASTNode* node) override;
    void visitFunctionCall(ASTNode* node) override;
    void visitArrayAccess(ASTNode* node) override;
    void visitDotIdentifier(ASTNode* node) override;
    void visitDotAccess(ASTNode* node) override;
    void visitFactor(ASTNode* node) override;
    void visitTerm(ASTNode* node) override;
    void visitArithExpr(ASTNode* node) override;
    void visitExpr(ASTNode* node) override;
    void visitImplementationId(ASTNode* node) override;
    void visitVariableId(ASTNode* node) override;
    void visitParamList(ASTNode* node) override;
    void visitParamId(ASTNode* node) override;
    void visitFloat(ASTNode* node) override;
    void visitInt(ASTNode* node) override;
    void visitStatementsList(ASTNode* node) override;
    void visitImplementationFunctionList(ASTNode* node) override;
    void visitCondition(ASTNode* node) override;
    void visitArrayType(ASTNode* node) override;
    void visitEmpty(ASTNode* node) override;
    void visitIndexList(ASTNode* node) override;
    void visitDimList(ASTNode* node) override;

private:
    // Symbol table and tracking
    std::shared_ptr<SymbolTable> symbolTable;
    std::shared_ptr<SymbolTable> currentTable;
    
    // Code generation context
    std::string currentClass;
    std::string currentFunction;
    
    // Generated code sections
    std::string dataSection;
    std::string codeSection;
    
    // Register allocation
    static const int NUM_REGS = 16;
    bool registerUsed[NUM_REGS];
    std::unordered_map<std::string, int> varToRegMap;
    
    // Label generation
    int labelCounter;
    int stringLiteralCounter;
    
    // Expression evaluation
    std::stack<int> exprStack;  // Stack of registers containing expression results
    
    // Helper methods for code generation
    
    // Register allocation
    int allocateRegister();
    void freeRegister(int reg);
    void freeAllRegisters();
    
    // Label generation
    std::string generateLabel(const std::string& prefix);
    std::string generateStringLabel();
    
    // Memory operations
    void loadVariable(const std::string& varName, int targetReg);
    void storeVariable(const std::string& varName, int sourceReg);
    void loadAddress(const std::string& varName, int targetReg);
    void loadLiteral(int value, int targetReg);
    void loadFloatLiteral(float value, int targetReg);
    
    // Function calls
    void generateFunctionPrologue(const std::string& funcName);
    void generateFunctionEpilogue();
    
    // Operations
    void generateBinaryOp(const std::string& op, int destReg, int leftReg, int rightReg);
    void generateRelOp(const std::string& op, int leftReg, int rightReg, const std::string& trueLabel, const std::string& falseLabel);
    
    // Stack operations
    void pushRegister(int reg);
    void popRegister(int reg);
    
    // Data section management
    void addToDataSection(const std::string& label, const std::string& data);
    void addStringLiteral(const std::string& str, std::string& label);
    
    // Utility methods
    int getSymbolOffset(const std::string& name);
    int getSymbolSize(const std::string& name);
    bool isLocalVariable(const std::string& name);
    std::string formatInstructionWithComments(const std::string& instruction, const std::string& comment);
    
    // Code emission
    void emit(const std::string& code);
    void emitComment(const std::string& comment);
    void emitLabel(const std::string& label);

    std::stack<int> registerPool;

    // Symbol metadata helpers
    std::string getSymbolTempVarKind(const std::string& name);

    // SymbolTable metadata helpers
    int getScopeOffset(std::shared_ptr<SymbolTable> table);
    void setScopeOffset(std::shared_ptr<SymbolTable> table, int offset);

    int getNodeOffset(ASTNode* node);
};

#endif // CODE_GEN_VISITOR_H