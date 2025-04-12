#ifndef SEMANTIC_CHECKING_VISITOR_H
#define SEMANTIC_CHECKING_VISITOR_H

#include "Visitor.h"
#include "SymbolTableVisitor.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

// Structure to store error information with location
struct SemanticError {
    std::string message;
    int line;
    bool isWarning;
};

// Type information structure for expression type tracking
struct TypeInfo {
    std::string type;
    std::vector<int> dimensions;
    bool isClassType = false;
};

class SemanticCheckingVisitor : public Visitor {
public:
    SemanticCheckingVisitor(std::shared_ptr<SymbolTable> globalTable);
    virtual ~SemanticCheckingVisitor();

    // Visit methods inherited from Visitor
    void visitEmpty(ASTNode* node) override;
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
    void visitIndexList(ASTNode* node) override;
    void visitDimList(ASTNode* node) override;

    // Helper methods for semantic checking
    void reportError(const std::string& message, ASTNode* node);
    void reportWarning(const std::string& message, ASTNode* node);
    void reportError(const std::string& message, int line);
    void reportWarning(const std::string& message, int line);
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    
    // New method to output errors to a file
    void outputErrors(const std::string& originalFilename);

    /**
     * @brief Imports errors from the symbol table visitor
     * @param symbolTableVisitor The visitor containing errors to import
     */
    void importSymbolTableErrors(const SymbolTableVisitor& symbolTableVisitor);

private:
    
    std::shared_ptr<SymbolTable> globalTable;
    std::shared_ptr<SymbolTable> currentTable;
    std::string currentClassName;
    std::string currentFunctionName;
    std::string currentType;
    std::vector<int> currentArrayDimensions;
    std::vector<std::string> expectedReturnType;
    
    // For tracking types during expression evaluation
    TypeInfo currentExprType;
    
    // Modified error and warning tracking to include location information
    std::vector<SemanticError> semanticErrors;
    
    // Helper methods
    TypeInfo getVariableType(const std::string& name);
    TypeInfo getFunctionReturnType(const std::string& name, const std::vector<TypeInfo>& paramTypes);
    TypeInfo getClassMemberType(const std::string& className, const std::string& memberName);
    bool areTypesCompatible(const TypeInfo& type1, const TypeInfo& type2);
    bool isNumericType(const std::string& type);
    bool checkClassCircularDependency(const std::string& className, std::unordered_set<std::string>& visited);
    
    // Type parsing utilities
    TypeInfo parseTypeString(const std::string& typeStr);
    std::string formatTypeInfo(const TypeInfo& typeInfo);

    // Check for duplicate class declarations
    void checkDuplicateClassDeclarations();

    // Check for duplicate function declarations in global scope
    void checkDuplicateFunctionDeclarations();

    // Check for duplicate member declarations in class
    void checkDuplicateMemberDeclarations(const std::string& className);

    // Check for shadowed inherited members
    void checkShadowedInheritedMembers(const std::string& className);

    // Check for undefined member functions (declared but not implemented)
    void checkUndefinedMemberFunctions();

    // Check for undeclared member functions (implemented but not declared)
    void checkUndeclaredMemberFunctions();

    // Map to store implementation function names for each class
    std::unordered_map<std::string, std::unordered_set<std::string>> implementedFunctions;

    // Map to store declaration function names for each class
    std::unordered_map<std::string, std::unordered_set<std::string>> declaredFunctions;

    std::shared_ptr<Symbol> lookupFunctionCaseInsensitive(const std::string& name);

    void handleMethodCall(const TypeInfo& objType, ASTNode* methodCallNode);
};

#endif // SEMANTIC_CHECKING_VISITOR_H