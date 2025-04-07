#ifndef VISITOR_H
#define VISITOR_H

#include "ASTGenerator/ASTNode.h"
#include <iostream>

/**
 * @class Visitor
 * @brief Abstract base class for visitors that traverse the AST
 */
class Visitor {
public:
    virtual ~Visitor() = default;

    // Visit methods for all node types in the AST
    virtual void visitEmpty(ASTNode* node) = 0;
    virtual void visitProgram(ASTNode* node) = 0;
    virtual void visitClassList(ASTNode* node) = 0;
    virtual void visitFunctionList(ASTNode* node) = 0;
    virtual void visitImplementationList(ASTNode* node) = 0;
    virtual void visitClass(ASTNode* node) = 0;
    virtual void visitFunction(ASTNode* node) = 0;
    virtual void visitImplementation(ASTNode* node) = 0;
    virtual void visitClassId(ASTNode* node) = 0;
    virtual void visitInheritanceList(ASTNode* node) = 0;
    virtual void visitInheritanceId(ASTNode* node) = 0;
    virtual void visitVisibility(ASTNode* node) = 0;
    virtual void visitMemberList(ASTNode* node) = 0;
    virtual void visitMember(ASTNode* node) = 0;
    virtual void visitVariable(ASTNode* node) = 0;
    virtual void visitFunctionId(ASTNode* node) = 0;
    virtual void visitFunctionSignature(ASTNode* node) = 0;
    virtual void visitFunctionBody(ASTNode* node) = 0;
    virtual void visitConstructorSignature(ASTNode* node) = 0;
    virtual void visitLocalVariable(ASTNode* node) = 0;
    virtual void visitBlock(ASTNode* node) = 0;
    virtual void visitIfStatement(ASTNode* node) = 0;
    virtual void visitWhileStatement(ASTNode* node) = 0;
    virtual void visitRelationalExpr(ASTNode* node) = 0;
    virtual void visitAssignment(ASTNode* node) = 0;
    virtual void visitFunctionDeclaration(ASTNode* node) = 0;
    virtual void visitAttribute(ASTNode* node) = 0;
    virtual void visitSingleStatement(ASTNode* node) = 0;
    virtual void visitExpressionStatement(ASTNode* node) = 0;
    virtual void visitReadStatement(ASTNode* node) = 0;
    virtual void visitWriteStatement(ASTNode* node) = 0;
    virtual void visitReturnStatement(ASTNode* node) = 0;
    virtual void visitAssignOp(ASTNode* node) = 0;
    virtual void visitRelOp(ASTNode* node) = 0;
    virtual void visitAddOp(ASTNode* node) = 0;
    virtual void visitMultOp(ASTNode* node) = 0;
    virtual void visitIdentifier(ASTNode* node) = 0;
    virtual void visitSelfIdentifier(ASTNode* node) = 0;
    virtual void visitType(ASTNode* node) = 0;
    virtual void visitArrayDimension(ASTNode* node) = 0;
    virtual void visitParam(ASTNode* node) = 0;
    virtual void visitFunctionCall(ASTNode* node) = 0;
    virtual void visitArrayAccess(ASTNode* node) = 0;
    virtual void visitDotIdentifier(ASTNode* node) = 0;
    virtual void visitDotAccess(ASTNode* node) = 0; // Add new method for DOT_ACCESS
    virtual void visitFactor(ASTNode* node) = 0;
    virtual void visitTerm(ASTNode* node) = 0;
    virtual void visitArithExpr(ASTNode* node) = 0;
    virtual void visitExpr(ASTNode* node) = 0;
    virtual void visitImplementationId(ASTNode* node) = 0;
    virtual void visitVariableId(ASTNode* node) = 0;
    virtual void visitParamList(ASTNode* node) = 0;
    virtual void visitParamId(ASTNode* node) = 0;
    virtual void visitFloat(ASTNode* node) = 0;
    virtual void visitInt(ASTNode* node) = 0;
    virtual void visitStatementsList(ASTNode* node) = 0;
    virtual void visitImplementationFunctionList(ASTNode* node) = 0;
    virtual void visitCondition(ASTNode* node) = 0;
    virtual void visitArrayType(ASTNode* node) = 0;

    // Generic visit method to dispatch based on node type
    virtual void visit(ASTNode* node) {
        switch (node->getNodeEnum()) {
            case NodeType::EMPTY: visitEmpty(node); break;
            case NodeType::PROGRAM: visitProgram(node); break;
            case NodeType::CLASS_LIST: visitClassList(node); break;
            case NodeType::FUNCTION_LIST: visitFunctionList(node); break;
            case NodeType::IMPLEMENTATION_LIST: visitImplementationList(node); break;
            case NodeType::CLASS: visitClass(node); break;
            case NodeType::FUNCTION: visitFunction(node); break;
            case NodeType::IMPLEMENTATION: visitImplementation(node); break;
            case NodeType::CLASS_ID: visitClassId(node); break;
            case NodeType::INHERITANCE_LIST: visitInheritanceList(node); break;
            case NodeType::INHERITANCE_ID: visitInheritanceId(node); break;
            case NodeType::VISIBILITY: visitVisibility(node); break;
            case NodeType::MEMBER_LIST: visitMemberList(node); break;
            case NodeType::MEMBER: visitMember(node); break;
            case NodeType::VARIABLE: visitVariable(node); break;
            case NodeType::FUNCTION_ID: visitFunctionId(node); break;
            case NodeType::FUNCTION_SIGNATURE: visitFunctionSignature(node); break;
            case NodeType::FUNCTION_BODY: visitFunctionBody(node); break;
            case NodeType::CONSTRUCTOR_SIGNATURE: visitConstructorSignature(node); break;
            case NodeType::LOCAL_VARIABLE: visitLocalVariable(node); break;
            case NodeType::BLOCK: visitBlock(node); break;
            case NodeType::IF_STATEMENT: visitIfStatement(node); break;
            case NodeType::WHILE_STATEMENT: visitWhileStatement(node); break;
            case NodeType::RELATIONAL_EXPR: visitRelationalExpr(node); break;
            case NodeType::ASSIGNMENT: visitAssignment(node); break;
            case NodeType::FUNCTION_DECLARATION: visitFunctionDeclaration(node); break;
            case NodeType::ATTRIBUTE: visitAttribute(node); break;
            case NodeType::SINGLE_STATEMENT: visitSingleStatement(node); break;
            case NodeType::EXPRESSION_STATEMENT: visitExpressionStatement(node); break;
            case NodeType::READ_STATEMENT: visitReadStatement(node); break;
            case NodeType::WRITE_STATEMENT: visitWriteStatement(node); break;
            case NodeType::RETURN_STATEMENT: visitReturnStatement(node); break;
            case NodeType::ASSIGN_OP: visitAssignOp(node); break;
            case NodeType::REL_OP: visitRelOp(node); break;
            case NodeType::ADD_OP: visitAddOp(node); break;
            case NodeType::MULT_OP: visitMultOp(node); break;
            case NodeType::IDENTIFIER: visitIdentifier(node); break;
            case NodeType::SELF_IDENTIFIER: visitSelfIdentifier(node); break;
            case NodeType::TYPE: visitType(node); break;
            case NodeType::ARRAY_DIMENSION: visitArrayDimension(node); break;
            case NodeType::PARAM: visitParam(node); break;
            case NodeType::FUNCTION_CALL: visitFunctionCall(node); break;
            case NodeType::ARRAY_ACCESS: visitArrayAccess(node); break;
            case NodeType::DOT_IDENTIFIER: visitDotIdentifier(node); break;
            case NodeType::DOT_ACCESS: visitDotAccess(node); break; // Add new case for DOT_ACCESS
            case NodeType::FACTOR: visitFactor(node); break;
            case NodeType::TERM: visitTerm(node); break;
            case NodeType::ARITH_EXPR: visitArithExpr(node); break;
            case NodeType::EXPR: visitExpr(node); break;
            case NodeType::IMPLEMENTATION_ID: visitImplementationId(node); break;
            case NodeType::VARIABLE_ID: visitVariableId(node); break;
            case NodeType::PARAM_LIST: visitParamList(node); break;
            case NodeType::PARAM_ID: visitParamId(node); break;
            case NodeType::FLOAT: visitFloat(node); break;
            case NodeType::INT: visitInt(node); break;
            case NodeType::STATEMENTS_LIST: visitStatementsList(node); break;
            case NodeType::IMPLEMENTATION_FUNCTION_LIST: visitImplementationFunctionList(node); break;
            case NodeType::CONDITION: visitCondition(node); break;
            case NodeType::ARRAY_TYPE: visitArrayType(node); break;
        }
    }
};

#endif // VISITOR_H