#include "../includes/lexer.hpp"
#include <cstddef>
#include <string_view>
#include <vector>

Lexer::Lexer(std::string_view source) : source(source) {}

char Lexer::peek(size_t offset) const {
  if (position + offset >= source.size())
    return '\0';

  return source[offset + position];
}

char Lexer::advance() {
  if (atEnd())
    return '\0';

  char c = source[position++];
  if (c == '\n') {
    line++;
    column = 1;
  } else {
    column++;
  }

  return c;
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  Token token;

  do {
    token = nextToken();
    tokens.push_back(token);
  } while (token.type != TokenType::End);

  return tokens;
}

Token Lexer::nextToken() {
  while (!atEnd()) {

    if (isWhitespace(peek())) {
      skipWhitespace();
    }

    else if (isComment()) {
      skipComment();
    } else {
      break;
    }
  }

  size_t start = position;
  size_t startLine = line;
  size_t startColumn = column;

  if (atEnd())
    return Token{TokenType::End, "", startLine, startColumn};

  char c = source.at(position);

    if (c == '"') {
    advance();
    size_t contentStart = position;

    while (!atEnd() && peek() != '"' && peek() != '\n')
      advance();

    if (atEnd() || peek() != '"')
      return Token{TokenType::Invalid, source.substr(start, position - start),
                   startLine, startColumn};

    std::string_view text = source.substr(contentStart, position - contentStart);
    advance();

    return Token{TokenType::String, text, startLine, startColumn};
  }

    if (isDigit(c)) {
    while (!atEnd() && isDigit(peek()))
      advance();

    if (peek() == '.' && isDigit(peek(1))) {
      advance();
      while (!atEnd() && isDigit(peek()))
        advance();
    }

    return Token{TokenType::Number, source.substr(start, position - start),
                 startLine, startColumn};
  } else if (isAlphaNumeric(c)) {    while (!atEnd() && isAlphaNumeric(peek())) {
      advance();
    }

    std::string_view text = source.substr(start, position - start);

    return Token{
        getKeywordType(text),
        text,
        startLine,
        startColumn,
    };
  }

  switch (peek()) {
  case '+':
    advance();

    return Token{
        TokenType::Plus,
        source.substr(start, 1),
        startLine,
        startColumn,
    };
  case '-':
    advance();

    return Token{
        TokenType::Minus,
        source.substr(start, 1),
        startLine,
        startColumn,
    };
  case '*':
    advance();

    return Token{
        TokenType::Star,
        source.substr(start, 1),
        startLine,
        startColumn,
    };
  case '/':
    advance();

    return Token{
        TokenType::Slash,
        source.substr(start, 1),
        startLine,
        startColumn,
    };

  case '=':
    advance();

    return Token{
        TokenType::Equals,
        source.substr(start, 1),
        startLine,
        startColumn,
    };
  case ':':
    advance();

    return Token{
        TokenType::Colon,
        source.substr(start, 1),
        startLine,
        startColumn,
    };
  case ';':
    advance();

    return Token{
        TokenType::Semicolon,
        source.substr(start, 1),
        startLine,
        startColumn,
    };
  case '(':
    advance();

    return Token{
      TokenType::LeftParen,
      "(",
      startLine,
      startColumn,
    };
  case ')':
    advance();
    
    return Token{
      TokenType::RightParen,
      ")",
      startLine,
      startColumn,
    };

  default:
    advance();

    return Token{TokenType::Invalid, "", startLine, startColumn};
  }

  return Token{TokenType::End, "", startLine, startColumn};
}

// Void

void Lexer::skipWhitespace() {
  while (!atEnd() && isWhitespace(peek())) {
    advance();
  }
}

void Lexer::skipComment() {
  advance();

  while (!atEnd() && peek() != '\n')
    advance();

  if (!atEnd())
    advance();
}

// Bool
bool Lexer::atEnd() const { return position >= source.size(); }

bool Lexer::isWhitespace(char c) const {
  return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

bool Lexer::isDigit(char c) const { return c >= '0' && c <= '9'; }
bool Lexer::isAlphaNumeric(char c) const {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' ||
         isDigit(c);
}

bool Lexer::isComment() const { return !atEnd() && peek() == ';'; }

TokenType getKeywordType(std::string_view text) {
  if (text == "true" || text == "false")
    return TokenType::Boolean;
  if (text == "print")
    return TokenType::KwPrint;

  return TokenType::Identifier;
}

std::string_view tokenTypeToString(TokenType type) {
  switch (type) {
  case TokenType::Number:
    return "Number";
  case TokenType::String:
    return "String";
  case TokenType::Boolean:
    return "Boolean";

  case TokenType::Identifier:
    return "Identifier";

  case TokenType::Plus:
    return "Plus";
  case TokenType::Minus:
    return "Minus";
  case TokenType::Star:
    return "Star";
  case TokenType::Slash:
    return "Slash";

  case TokenType::Equals:
    return "Equals";
  case TokenType::LeftParen:
    return "LeftParen";
  case TokenType::RightParen:
    return "RightParen";

  case TokenType::Semicolon:
    return "Semicolon";
  case TokenType::Colon:
    return "Colon";

  case TokenType::KwPrint:
    return "Print";

  case TokenType::End:
    return "End";

  case TokenType::Invalid:
    return "Invalid";
  }

  return "Unknown";
}
