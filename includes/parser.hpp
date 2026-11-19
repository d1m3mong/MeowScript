#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"
#include "types.hpp"

class ParseError : public std::runtime_error {
public:
  ParseError(const std::string &message, const Token &token);
};

class Parser {
  std::vector<Token> tokens;
  size_t position{0};
  std::vector<ASTNode> ast;

  int getBindingPower(TokenType type) const;
  bool isOperator(TokenType type) const;

  std::optional<Token> peek(size_t offset = 0) const;
  std::optional<Token> previous() const;
  std::optional<Token> advance();

  bool check(TokenType type);

  bool match(TokenType type);
  std::optional<Token> expect(TokenType type, const std::string &message);

  bool atEnd() const;

  std::optional<ASTNode> parseExpression(int minBindingPower = 0);
  std::optional<ASTNode> parsePrimary();
  std::variant<std::int64_t, double> parseNumber(const Token &token);

  bool parseDeclaration();
 bool parseAssignment();
  TypeRef parseType();

  bool parseStatement();
  bool parsePrint();

  double transformCharToDigit(const Token &token);
  bool tranformCharToBoolean(const Token &token);

public:
  explicit Parser(std::vector<Token> &tokens) : tokens(std::move(tokens)) {}
  bool parse();

  void showAST(const ASTNode *node) const;
  const std::vector<ASTNode> &getAST() const;
};
