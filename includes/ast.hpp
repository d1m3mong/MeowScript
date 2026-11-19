#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "lexer.hpp"
#include "types.hpp"

struct ASTNode;
using NodePtr = std::unique_ptr<ASTNode>;

struct NumberNode {
  std::variant<std::int64_t, double> value;
};

struct BoolNode {
  bool value;
};

struct StringNode {
  std::string value;
};

struct IdentifierNode {
  std::string name;
};

struct UnaryExprNode {
  TokenType op;
  NodePtr operand;
};

struct BinaryExprNode {
  NodePtr left;
  NodePtr right;
  TokenType op;
};

struct DeclarationNode {
  std::string name;
  TypeRef type;
  NodePtr initializer;
};

struct AssignmentNode {
  std::string name;
  NodePtr value;
};

struct PrintNode {
  NodePtr content;
};

struct BlockNode {
  std::vector<NodePtr> statements;
};

struct ASTNode {
  std::variant<NumberNode, BoolNode, StringNode, IdentifierNode, UnaryExprNode,
               BinaryExprNode, DeclarationNode, AssignmentNode, PrintNode,
               BlockNode>
      value;
};

template <typename T> NodePtr makeNode(T node) {
  return std::make_unique<ASTNode>(ASTNode{std::move(node)});
}
