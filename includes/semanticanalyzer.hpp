#pragma once

#include "ast.hpp"
#include "symboltable.hpp"
#include <vector>

class ScopeGuard {
  SymbolTable &symbols;

public:
  explicit ScopeGuard(SymbolTable &symbols) : symbols(symbols) {
    symbols.enterScope();
  }

  ~ScopeGuard() { symbols.exitScope(); }

  ScopeGuard(const ScopeGuard &) = delete;
  ScopeGuard &operator=(const ScopeGuard &) = delete;
};

class SemanticAnalyzer {
  SymbolTable symbols;

  void analyzeDeclaration(const DeclarationNode &declaration);
  TypeRef analyzeIdentifier(const IdentifierNode &identifier);
  void analyzeAssignment(const AssignmentNode &assignment);
  TypeRef analyzeExpression(const ASTNode &expression);
  TypeRef getBinaryResultType(TypeRef left, TypeRef right, TokenType op);
  void analyzeStatement(const ASTNode &node);
  void analyzeBlock(const BlockNode &block);
  bool isNumeric(PrimitiveKind type);
  TypeRef analyzeUnaryExpression(const UnaryExprNode &expression);

public:
  void analyze(const std::vector<ASTNode> &ast);
};
