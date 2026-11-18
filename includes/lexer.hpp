#pragma once

#include <string_view>
#include <vector>
enum class TokenType {
  Number,
  String,
  Boolean,

  Identifier,

  Plus,
  Minus,
  Star,
  Slash,

  Colon,
  Semicolon,

  Equals,

  KwPrint,

  End,
  Invalid,
};

struct Token {
  TokenType type;
  std::string_view text;
  size_t line;
  size_t column;
};

TokenType getKeywordType(std::string_view text);
std::string_view tokenTypeToString(TokenType type);

class Lexer {
  std::string_view source;
  size_t position{0};
  size_t line{1};
  size_t column{1};

  bool isAlphaNumeric(char c) const;
  bool isDigit(char c) const;

  bool atEnd() const;

  bool isWhitespace(char c) const;
  void skipWhitespace();
  bool isComment() const;
  void skipComment();

  char peek(size_t offset = 0) const;
  char advance();

public:
  Lexer(std::string_view source);
  Token nextToken();

  std::vector<Token> tokenize();
};
