#include "../includes/semanticanalyzer.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

#include <iostream>

void SemanticAnalyzer::analyzeDeclaration(const DeclarationNode &declaration) {
  if (declaration.initializer) {
    TypeRef initializerType = analyzeExpression(*declaration.initializer);

    std::cout << "Initializer: " << initializerType.text << '\n';
    std::cout << "Declaration: " << declaration.type.text << '\n';

    if (initializerType != declaration.type)
      throw std::runtime_error("Variable declaration type mismatch!");
  }

  symbols.declare(Symbol{declaration.type, declaration.name});
}

TypeRef SemanticAnalyzer::analyzeIdentifier(const IdentifierNode &identifier) {
  const Symbol *symbol = symbols.lookup(identifier.name);
  return symbol->type;
}

void SemanticAnalyzer::analyzeAssignment(const AssignmentNode &assignment) {
  const Symbol *symbol = symbols.lookup(assignment.name);

  TypeRef valueType = analyzeExpression(*assignment.value);

  if (symbol->type != valueType)
    throw std::runtime_error("Assignment type mismatch!");
}

TypeRef SemanticAnalyzer::getBinaryResultType(TypeRef left, TypeRef right,
                                              TokenType op) {
  if (left.primitive != right.primitive)
    throw std::runtime_error("Invalid operation on different types!");

  switch (op) {
  case TokenType::Plus:
  case TokenType::Minus:
  case TokenType::Star:
  case TokenType::Slash:
    if (isNumeric(left.primitive.value()))
      return left;

    break;
  case TokenType::EqualEqual:
  case TokenType::NotEqual:
    return TypeRef{"bool", PrimitiveKind::Bool};
  case TokenType::smaller:
  case TokenType::bigger:
  case TokenType::biggerAnd:
  case TokenType::smallerAnd:
    if (isNumeric(left.primitive.value()))
      return TypeRef{"bool", PrimitiveKind::Bool};

  default:
    break;
  }

  throw std::runtime_error("Invalid Operator!");
}

TypeRef
SemanticAnalyzer::analyzeUnaryExpression(const UnaryExprNode &expression) {
  TypeRef operandType = analyzeExpression(*expression.operand);

  if (!operandType.primitive.has_value())
    throw std::runtime_error("Invalid operand types!");

  switch (expression.op) {
  case TokenType::Minus:
    if (isNumeric(operandType.primitive.value()))
      return operandType;
    break;
  case TokenType::Bang:
    if (operandType.primitive == PrimitiveKind::Bool)
      return operandType;
    break;

  default:
    break;
  }

  throw std::runtime_error("Invalid unary Operation!");
}

TypeRef SemanticAnalyzer::analyzeExpression(const ASTNode &expression) {
  return std::visit(
      [this](const auto &value) -> TypeRef {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, BinaryExprNode>) {
          auto left = analyzeExpression(*value.left);
          auto right = analyzeExpression(*value.right);

          if (left.primitive != right.primitive)
            throw std::runtime_error("Type mismatch in binary expression!");

          return left;
        } else if constexpr (std::is_same_v<T, UnaryExprNode>) {
          auto operand = analyzeExpression(*value.operand);

          if (value.op == TokenType::Minus) {
            if (operand.primitive != PrimitiveKind::I64 &&
                operand.primitive != PrimitiveKind::F64)
              throw std::runtime_error("Unary minus requires numeric operand!");
          }

          return operand;

        } else if constexpr (std::is_same_v<T, IdentifierNode>) {
          return analyzeIdentifier(value);
        } else if constexpr (std::is_same_v<T, NumberNode>) {
          if (std::holds_alternative<std::int64_t>(value.value))
            return TypeRef{"i64", PrimitiveKind::I64};

          return TypeRef{"f64", PrimitiveKind::F64};
        } else if constexpr (std::is_same_v<T, StringNode>) {
          return TypeRef{"string", PrimitiveKind::String};
        } else if constexpr (std::is_same_v<T, BoolNode>) {
          return TypeRef{"bool", PrimitiveKind::Bool};
        } else if constexpr (std::is_same_v<T, UnaryExprNode>) {
          return analyzeUnaryExpression(value);
        }

        throw std::runtime_error("Unsupported expression!");
      },
      expression.value);
}

void SemanticAnalyzer::analyzeStatement(const ASTNode &node) {
  std::visit(
      [this](const auto &value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, AssignmentNode>) {
          analyzeAssignment(value);
        } else if constexpr (std::is_same_v<T, PrintNode>) {
          analyzeExpression(*value.content);
        } else if constexpr (std::is_same_v<T, DeclarationNode>) {
          analyzeDeclaration(value);
        } else if constexpr (std::is_same_v<T, BlockNode>) {
          analyzeBlock(value);
        }
      },
      node.value);
}

void SemanticAnalyzer::analyzeBlock(const BlockNode &block) {
  ScopeGuard guard(symbols);

  for (const auto &statement : block.statements)
    analyzeStatement(*statement);
}

void SemanticAnalyzer::analyze(const std::vector<ASTNode> &ast) {
  for (const auto &node : ast)
    analyzeStatement(node);
}

bool SemanticAnalyzer::isNumeric(PrimitiveKind type) {
  switch (type) {
  case PrimitiveKind::I8:
  case PrimitiveKind::I16:
  case PrimitiveKind::I32:
  case PrimitiveKind::I64:
  case PrimitiveKind::U8:
  case PrimitiveKind::U16:
  case PrimitiveKind::U32:
  case PrimitiveKind::U64:
  case PrimitiveKind::F32:
  case PrimitiveKind::F64:
    return true;
  default:
    return false;
  }
}
