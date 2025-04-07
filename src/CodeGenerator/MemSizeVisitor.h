#ifndef MEM_SIZE_VISITOR_H
#define MEM_SIZE_VISITOR_H

#include "Semantics/Visitor.h"
#include "Semantics/SymbolTableVisitor.h"
#include <stack>
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>

// Define data type sizes
enum TypeSize {
    VOID_SIZE = 0,
    INT_SIZE = 4,
    FLOAT_SIZE = 8,
    POINTER_SIZE = 4
};

// Memory Size Allocator Visitor Class
class MemSizeVisitor : public Visitor {
public:
    MemSizeVisitor(std::shared_ptr<SymbolTable> symbolTable);
    ~MemSizeVisitor();

    // Methods for memory calculation
    void processAST(ASTNode* root);
    void calculateMemorySizes();
    
    // Method to output the updated symbol table
    void outputSymbolTable(const std::string& filename);
    
    // Visitor pattern implementation
    void visitProgram(ASTNode* node) override;
    void visitClassList(ASTNode* node) override;
    void visitClass(ASTNode* node) override;
    void visitClassId(ASTNode* node) override;
    void visitInheritanceList(ASTNode* node) override;
    void visitInheritanceId(ASTNode* node) override;
    void visitVisibility(ASTNode* node) override;
    void visitMemberList(ASTNode* node) override;
    void visitFunction(ASTNode* node) override;
    void visitFunctionBody(ASTNode* node) override;
    void visitEmpty(ASTNode* node) override;
    void visitFunctionList(ASTNode* node) override;
    void visitImplementationList(ASTNode* node) override;
    void visitImplementation(ASTNode* node) override;
    void visitImplementationFunctionList(ASTNode* node) override;
    void visitMember(ASTNode* node) override;
    void visitVariable(ASTNode* node) override;
    void visitVariableId(ASTNode* node) override;
    void visitLocalVariable(ASTNode* node) override;
    void visitFunctionId(ASTNode* node) override;
    void visitFunctionSignature(ASTNode* node) override;
    void visitConstructorSignature(ASTNode* node) override;
    void visitParamList(ASTNode* node) override;
    void visitParam(ASTNode* node) override;
    void visitParamId(ASTNode* node) override;
    void visitType(ASTNode* node) override;
    void visitArrayType(ASTNode* node) override;
    void visitArrayDimension(ASTNode* node) override;
    void visitBlock(ASTNode* node) override;
    void visitStatementsList(ASTNode* node) override;
    void visitIfStatement(ASTNode* node) override;
    void visitWhileStatement(ASTNode* node) override;
    void visitCondition(ASTNode* node) override;
    void visitSingleStatement(ASTNode* node) override;
    void visitExpressionStatement(ASTNode* node) override;
    void visitAssignment(ASTNode* node) override;
    void visitReadStatement(ASTNode* node) override;
    void visitWriteStatement(ASTNode* node) override;
    void visitReturnStatement(ASTNode* node) override;
    void visitFunctionDeclaration(ASTNode* node) override;
    void visitIdentifier(ASTNode* node) override;
    void visitSelfIdentifier(ASTNode* node) override;
    void visitFunctionCall(ASTNode* node) override;
    void visitDotIdentifier(ASTNode* node) override;
    void visitDotAccess(ASTNode* node) override; // Add new method for DOT_ACCESS
    void visitArrayAccess(ASTNode* node) override;
    void visitRelationalExpr(ASTNode* node) override;
    void visitExpr(ASTNode* node) override;
    void visitArithExpr(ASTNode* node) override;
    void visitTerm(ASTNode* node) override;
    void visitFactor(ASTNode* node) override;
    void visitAssignOp(ASTNode* node) override;
    void visitRelOp(ASTNode* node) override;
    void visitAddOp(ASTNode* node) override;
    void visitMultOp(ASTNode* node) override;
    void visitFloat(ASTNode* node) override;
    void visitInt(ASTNode* node) override;
    void visitAttribute(ASTNode* node) override;
    void visitImplementationId(ASTNode* node) override;

private:
    // Symbol table and tracking
    std::shared_ptr<SymbolTable> currentTable;
    std::shared_ptr<SymbolTable> symbolTable;
    
    // Context information
    std::string currentType;
    std::string currentClassName;
    std::string currentFunctionName;
    std::string currentOperator;
    
    // Expression handling
    int tempVarCounter;
    std::stack<std::string> expressionTypes;
    
    // Helper methods
    int getTypeSize(const std::string& type);
    void calculateTableOffsets(std::shared_ptr<SymbolTable> table);
    std::string createTempVar(const std::string& type, const std::string& kind = "tempvar");
    void writeTableToFile(std::ofstream& out, std::shared_ptr<SymbolTable> table, int indent);

    // Extensions to Symbol class
    void setSymbolSize(std::shared_ptr<Symbol> symbol, int size);
    void setSymbolOffset(std::shared_ptr<Symbol> symbol, int offset);
    void setSymbolTempVarKind(std::shared_ptr<Symbol> symbol, const std::string& kind);
    int getSymbolSize(std::shared_ptr<Symbol> symbol);
    int getSymbolOffset(std::shared_ptr<Symbol> symbol);
    std::string getSymbolTempVarKind(std::shared_ptr<Symbol> symbol);
    
    // Extensions to SymbolTable class
    void setTableScopeOffset(std::shared_ptr<SymbolTable> table, int offset);
    int getTableScopeOffset(std::shared_ptr<SymbolTable> table);
};

#endif // MEM_SIZE_VISITOR_H