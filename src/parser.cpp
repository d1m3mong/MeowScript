#include <charconv>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <variant>
#include <vector>

#include "../includes/parser.hpp"
#include "../includes/types.hpp"

int Parser::getBindingPower(TokenType type) const {
  switch (type) {
  case TokenType::Plus:
  case TokenType::Minus:
    return 10;
  case TokenType::Star:
  case TokenType::Slash:
    return 20;
  default:
    return 0;
  }
}

bool Parser::isOperator(TokenType type) const {
  switch (type) {
  case TokenType::Plus:
  case TokenType::Minus:
  case TokenType::Slash:
  case TokenType::Star:
    return true;

  default:
    return false;
  }
}

std::optional<Token> Parser::peek(size_t offset) const {
  if (position + offset >= tokens.size())
    return std::nullopt;

  return tokens[position + offset];
}

std::optional<Token> Parser::previous() const {
  if (position == 0)
    return std::nullopt;

  return tokens[position - 1];
}

std::optional<Token> Parser::advance() {
  if (position >= tokens.size())
    return std::nullopt;

  return tokens[position++];
}

bool Parser::check(TokenType type) {
  auto token = peek();
  return token && token->type == type;
}

bool Parser::match(TokenType type) {
  if (!check(type))
    return false;
  advance();
  return true;
}

std::optional<Token> Parser::expect(TokenType type,
                                    const std::string &message) {
  if (!check(type))
    throw ParseError(message, peek().value_or(tokens.back()));
  return advance().value();
}

bool Parser::atEnd() const {
  auto res = peek();
  return !res.has_value() || res->type == TokenType::End;
}

// parse

std::optional<ASTNode> Parser::parseExpression(int minBindingPower) {
  auto primary = parsePrimary();
  if (!primary)
    return std::nullopt;

  NodePtr left = std::make_unique<ASTNode>(std::move(*primary));

  while (true) {
    auto token = peek();
    if (!token)
      break;

    int bindingPower = getBindingPower(token->type);
    if (bindingPower == 0 || bindingPower < minBindingPower)
      break;

    TokenType op = advance()->type;

    auto rightResult = parseExpression(bindingPower + 1);
    if (!rightResult)
      return std::nullopt;

    NodePtr right = std::make_unique<ASTNode>(std::move(*rightResult));
    left = std::make_unique<ASTNode>(
        BinaryExprNode{std::move(left), std::move(right), op});
  }

  return std::move(*left);
}

std::optional<ASTNode> Parser::parsePrimary() {
  auto token = peek();
  if (!token)
    return std::nullopt;

  switch (token->type) {
  case TokenType::Number:
    advance();
    return ASTNode{NumberNode{parseNumber(*token)}};
  case TokenType::String:
    advance();
    return ASTNode{StringNode{std::string(token->text)}};
  case TokenType::Boolean:
    advance();
    return ASTNode{BoolNode{token->text == "true"}};
  case TokenType::Identifier:
    advance();
    return ASTNode{IdentifierNode{std::string(token->text)}};
  case TokenType::Minus: {
    advance();
    auto inner = parseExpression(30);
    if (!inner)
      return std::nullopt;
    return ASTNode{UnaryExprNode{TokenType::Minus,
                                 std::make_unique<ASTNode>(std::move(*inner))}};
  }
  case TokenType::LeftParen: {
    advance();

    auto inner = parseExpression();
    if (!inner)
      return std::nullopt;

    expect(TokenType::RightParen, "Expected ')'!");
    return inner;
  }
  default:
    throw ParseError("Expected expression", *token);
  }
}

std::optional<ASTNode> Parser::parseDeclaration() {
  std::string name(
      expect(TokenType::Identifier, "Expected variable name").value().text);
  expect(TokenType::Colon, "Expected ':' after variable name");
  TypeRef type = parseType();
  expect(TokenType::Equals, "Expected '=' in declaration");

  auto value = parseExpression();
  if (!value)
    throw ParseError("Expected expression",
                     peek().value_or(previous().value()));
  match(TokenType::Semicolon);

  ASTNode declaration{
      DeclarationNode{std::move(name), std::move(type),
                      std::make_unique<ASTNode>(std::move(*value))}};

  return declaration;
}

TypeRef Parser::parseType() {
  auto token = expect(TokenType::Identifier, "Expected type name!");

  return TypeRef{std::string(token.value().text),
                 lookupPrimitive(token.value().text)};
}

std::variant<std::int64_t, double> Parser::parseNumber(const Token &token) {
  const char *first = token.text.data();
  const char *last = first + token.text.size();

  if (token.text.find('.') == std::string_view::npos) {
    std::int64_t integer{};
    auto [ptr, ec] = std::from_chars(first, last, integer);
    if (ec == std::errc{} && ptr == last)
      return integer;
  }

  double real{};
  auto [ptr, ec] = std::from_chars(first, last, real);
  if (ec != std::errc{} || ptr != last)
    throw ParseError("Invalid number!", token);
  return real;
}

std::optional<ASTNode> Parser::parseStatement() {
  auto token = peek();
  if (!token)
    return std::nullopt;

  if (token->type == TokenType::KwPrint)
    return parsePrint();

  if (token->type == TokenType::LeftBrack)
    return parseBlock();

  if (token->type == TokenType::Identifier) {
    auto next = peek(1);

    if (next && next->type == TokenType::Colon)
      return parseDeclaration();

    if (next && next->type == TokenType::Equals)
      return parseAssignment();
  }

  return std::nullopt;
}

std::optional<ASTNode> Parser::parsePrint() {
  advance();

  if (!match(TokenType::LeftParen))
    throw ParseError("Expected '(' after print!",
                     peek().value_or(previous().value()));

  auto value = parseExpression();
  if (!value)
    throw ParseError("Expected expression",
                     peek().value_or(previous().value()));

  if (!match(TokenType::RightParen))
    throw ParseError("Expected ')' after print!",
                     peek().value_or(previous().value()));

  ASTNode print{PrintNode{std::make_unique<ASTNode>(std::move(*value))}};

  return print;
}

std::optional<ASTNode> Parser::parseBlock() {
  expect(TokenType::LeftBrack, "Expected '{'");

  std::vector<NodePtr> statements;

  while (!check(TokenType::RightBrack)) {
    if (atEnd())
      throw std::runtime_error("Expected '}'");

    auto statement = parseStatement();

    if (!statement)
      throw ParseError("Unexpected token in block!", peek().value());

    statements.push_back(std::make_unique<ASTNode>(std::move(*statement)));
  }

  advance(); // consume }

  return ASTNode{BlockNode{std::move(statements)}};
}

std::optional<ASTNode> Parser::parseAssignment() {
  std::string name(
      expect(TokenType::Identifier, "Expected variable name").value().text);
  expect(TokenType::Equals, "Expected '=' in assignment");

  auto value = parseExpression();
  if (!value)
    throw ParseError("Expected expression",
                     peek().value_or(previous().value()));
  match(TokenType::Semicolon);

  ASTNode declaration{AssignmentNode{
      std::move(name), std::make_unique<ASTNode>(std::move(*value))}};

  return declaration;
}

bool Parser::parse() {
  while (!atEnd()) {
    auto statement = parseStatement();

    if (!statement)
      throw ParseError("Unexpected token!", peek().value());

    ast.push_back(std::move(*statement));
  }

  return true;
}

double Parser::transformCharToDigit(const Token &token) {
  double value{};

  auto [ptr, ec] = std::from_chars(
      token.text.data(), token.text.data() + token.text.size(), value);

  if (ec != std::errc{} || ptr != token.text.data() + token.text.size())
    throw std::runtime_error("No valid Digit conversion!");

  return value;
}

bool Parser::tranformCharToBoolean(const Token &token) {
  if (token.text == "true")
    return true;
  return false;
}

void Parser::showAST(const ASTNode *node) const {
  std::visit(
      [this](const auto &value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, NumberNode>) {
          std::cout << "NumberNode\n";
        }
        if constexpr (std::is_same_v<T, StringNode>) {
          std::cout << "StringNode\n";
        }

        if constexpr (std::is_same_v<T, BoolNode>) {
          std::cout << "BooleanNode\n";
        }

        if constexpr (std::is_same_v<T, UnaryExprNode>) {
          std::cout << tokenTypeToString(value.op);
          showAST(value.operand.get());
          std::cout << '\n';
        }

        if constexpr (std::is_same_v<T, BinaryExprNode>) {
          std::cout << "(";
          showAST(value.left.get());
          std::cout << tokenTypeToString(value.op) << " ";
          showAST(value.right.get());
          std::cout << ")\n";
        }

        if constexpr (std::is_same_v<T, DeclarationNode>) {
          std::cout << "DeclarationNode: " << value.name << " type: ";

          if (value.type.primitive.has_value())
            std::cout << getPrimitiveSurname(*value.type.primitive);
          else
            std::cout << value.type.text;

          std::cout << '\n';
        }

        if constexpr (std::is_same_v<T, AssignmentNode>) {
          std::cout << "AssignmentNode: " << value.name << "\n";
        }
        if constexpr (std::is_same_v<T, PrintNode>) {
          std::cout << "PrintNode: ";
          showAST(value.content.get());
          std::cout << '\n';
        }
      },
      node->value);
}

const std::vector<ASTNode> &Parser::getAST() const { return ast; }

ParseError::ParseError(const std::string &message, const Token &token)
    : std::runtime_error("Line: " + std::to_string(token.line) + ", column " +
                         std::to_string(token.column) + ": " + message) {}
