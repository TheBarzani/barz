#ifndef SYMBOL_TABLE_VISITOR_H
#define SYMBOL_TABLE_VISITOR_H

#include "Visitor.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

// Forward declarations
class SymbolTable;
class Symbol;

enum class SymbolKind {
    CLASS,
    VARIABLE,
    FUNCTION,
    PARAMETER
};

enum class Visibility {
    PUBLIC,
    PRIVATE
};

// Error and warning tracking
struct ErrorInfo {
    std::string message;
    int line;
};

// For FUNCTION symbols
struct ParamInfo {
    std::string name;
    std::string type;
};

// Represents a function signature (name + parameter types)
struct FunctionSignature {
    std::string name;
    std::vector<std::string> paramTypes;
    
    bool operator==(const FunctionSignature& other) const;
};

// Hash function for FunctionSignature
namespace std {
    template<>
    struct hash<FunctionSignature> {
        std::size_t operator()(const FunctionSignature& sig) const;
    };
}

// Symbol class to represent entries in symbol table
class Symbol {
public:
    Symbol(const std::string& name, const std::string& type, SymbolKind kind);
    
    // Getters and setters
    std::string getName() const { return name; }
    std::string getType() const { return type; }
    SymbolKind getKind() const { return kind; }
    Visibility getVisibility() const { return visibility; }
    void setVisibility(Visibility vis) { visibility = vis; }
    
    // For CLASS symbols
    void addInheritedClass(const std::string& className) { inheritedClasses.push_back(className); }
    const std::vector<std::string>& getInheritedClasses() const { return inheritedClasses; }
    
    // For FUNCTION symbols
    void addParam(const std::string& name, const std::string& type) {
        params.push_back({name, type});
    }
    
    const std::vector<ParamInfo>& getParamInfo() const {
        return params;
    }
    
    // For backward compatibility
    std::vector<std::string> getParams() const {
        std::vector<std::string> types;
        for (const auto& param : params) {
            types.push_back(param.type);
        }
        return types;
    }
    
    bool isDefined() const { return defined; }
    void setDefined(bool def) { defined = def; }
    bool isDeclared() const { return declared; }
    void setDeclared(bool decl) { declared = decl; }
    
    // For VARIABLE symbols
    bool isArray() const { return !arrayDimensions.empty(); }
    void addArrayDimension(int dim) { arrayDimensions.push_back(dim); }
    const std::vector<int>& getArrayDimensions() const { return arrayDimensions; }

    void setDeclarationLine(int line) { declarationLine = line; }
    void setDefinitionLine(int line) { definitionLine = line; }
    int getDeclarationLine() const { return declarationLine; }
    int getDefinitionLine() const { return definitionLine; }

    // For backward compatibility - convert types to ParamInfo objects with generated names
    void setParams(const std::vector<std::string>& paramTypes) {
        // Clear existing params
        params.clear();
        
        // Convert types to ParamInfo objects with generic parameter names
        for (size_t i = 0; i < paramTypes.size(); i++) {
            params.push_back({"param" + std::to_string(i+1), paramTypes[i]});
        }
    }
    
    // Set params with both names and types
    void setParams(const std::vector<std::string>& paramNames, const std::vector<std::string>& paramTypes) {
        // Clear existing params
        params.clear();
        
        // Store name+type pairs
        for (size_t i = 0; i < paramNames.size() && i < paramTypes.size(); i++) {
            params.push_back({paramNames[i], paramTypes[i]});
        }
    }

private:
    std::string name;
    std::string type;
    SymbolKind kind;
    Visibility visibility = Visibility::PRIVATE;
    
    // For CLASS symbols
    std::vector<std::string> inheritedClasses;
    
    std::vector<ParamInfo> params;
    bool defined = false;
    bool declared = false;
    
    // For VARIABLE symbols
    std::vector<int> arrayDimensions;

    int declarationLine = 0;
    int definitionLine = 0;
};

// Symbol Table class to represent a single scope's symbols
class SymbolTable {
public:
    SymbolTable(const std::string& scopeName, SymbolTable* parent = nullptr);
    
    // Add a symbol to this table
    bool addSymbol(std::shared_ptr<Symbol> symbol);
    
    // Lookup a symbol in this table (or parent tables if not found)
    std::shared_ptr<Symbol> lookupSymbol(const std::string& name, bool localOnly = false);
    
    // Lookup a function with specific signature
    std::shared_ptr<Symbol> lookupFunction(const FunctionSignature& signature, bool localOnly = false);
    
    // Get all functions with a given name (for overload checking)
    std::vector<std::shared_ptr<Symbol>> lookupFunctions(const std::string& name, bool localOnly = false);
    
    // Add a nested table for a class or function
    void addNestedTable(const std::string& name, std::shared_ptr<SymbolTable> table);
    
    // Get a nested table
    std::shared_ptr<SymbolTable> getNestedTable(const std::string& name);
    
    // Get all symbols in this table
    const std::unordered_map<std::string, std::shared_ptr<Symbol>>& getSymbols() const { return symbols; }
    
    // Get all nested tables
    const std::unordered_map<std::string, std::shared_ptr<SymbolTable>>& getNestedTables() const { return nestedTables; }
    
    // Get scope name
    std::string getScopeName() const { return scopeName; }
    
    // Get parent table
    SymbolTable* getParent() const { return parent; }

    // Get all functions in this table
    std::vector<std::pair<FunctionSignature, std::shared_ptr<Symbol>>> getAllFunctions() const;

    void setFunctionSymbol(std::shared_ptr<Symbol> symbol) { functionSymbol = symbol; }
    std::shared_ptr<Symbol> getFunctionSymbol() const { return functionSymbol; }

private:
    std::string scopeName;
    SymbolTable* parent;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    std::unordered_map<std::string, std::shared_ptr<SymbolTable>> nestedTables;
    std::unordered_map<FunctionSignature, std::shared_ptr<Symbol>> functions;
    std::shared_ptr<Symbol> functionSymbol = nullptr;  // For function tables
};

// Symbol Table Visitor implementation
class SymbolTableVisitor : public Visitor {
public:
    SymbolTableVisitor();
    ~SymbolTableVisitor();
    
    // Output the symbol table to a file
    void outputSymbolTable(const std::string& filename);
    
    // Get the generated global symbol table
    std::shared_ptr<SymbolTable> getGlobalTable() const { return globalTable; }

    // Implementation of all visitor methods
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

    /**
     * @brief Gets all errors reported during symbol table generation
     * @return Vector of error messages
     */
    const std::vector<ErrorInfo>& getErrors() const { return errors; }
    
    /**
     * @brief Gets all warnings reported during symbol table generation
     * @return Vector of warning messages
     */
    const std::vector<ErrorInfo>& getWarnings() const { return warnings; }

private:
    // Helper methods
    void reportError(const std::string& message, ASTNode* node);
    void reportError(const std::string& message, int line);
    void reportWarning(const std::string& message, ASTNode* node);
    void reportWarning(const std::string& message, int line);
    void writeTableToFile(std::ofstream& out, std::shared_ptr<SymbolTable> table, int indent = 0);
    
    // Check for function declaration/definition consistency
    void checkFunctionConsistency();
    
    // Check for shadowed members
    void checkShadowedMembers(const std::string& className);

    std::string formattedParamList(const std::vector<std::string>& params);
    std::string formattedParamListForTableName(const std::vector<std::string>& params);
    std::string formatFunctionParams(std::shared_ptr<Symbol> funcSymbol);
    
    // Data members
    std::shared_ptr<SymbolTable> globalTable;
    std::shared_ptr<SymbolTable> currentTable;
    
    // Current context tracking
    std::string currentClassName;
    std::string currentFunctionName;
    Visibility currentVisibility;
    std::string currentType;
    std::vector<std::string> currentParamTypes;
    std::vector<int> currentArrayDimensions;
    std::vector<std::string> currentParamNames;
    
    std::vector<ErrorInfo> errors;
    std::vector<ErrorInfo> warnings;

    std::string formatTypeWithDimensions(const std::shared_ptr<Symbol>& symbol) const;

    void createFunctionSubtable(std::shared_ptr<SymbolTable> parentTable, 
                          const std::string& prefix,
                          const std::string& funcName,
                          std::shared_ptr<Symbol> funcSymbol);
    std::string formatParamTypesForTableName(const std::vector<std::string>& paramTypes);
};

#endif // SYMBOL_TABLE_VISITOR_H