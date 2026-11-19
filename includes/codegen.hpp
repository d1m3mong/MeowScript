#pragma once

#include <string>
#include <vector>

#include "parser.hpp"

class CodeGen {
  static std::string cppTypes(const TypeRef &t);
  std::string expression(const ASTNode *node);
  std::string statement(const ASTNode *node);

public:
  std::string generate(const std::vector<ASTNode> &program);
};

static std::string number(const std::variant<std::int64_t, double> &v);
static std::string escape(const std::string &input);
static std::string operatorText(TokenType op);
