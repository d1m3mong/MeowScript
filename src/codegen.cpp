#include "../includes/codegen.hpp"
#include <charconv>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

std::string CodeGen::generate(const std::vector<ASTNode> &program) {
  std::string body;
  for (const auto &node : program)
    body += " " + statement(&node) + '\n';

  return "#include <cstdint>\n"
         "#include <iostream>\n"
         "#include <string>\n"
         "inline int meow_show(std::uint8_t v) { return v; }\n"
         "inline int meow_show(std::uint16_t v) { return v; }\n"
         "template<class T> const T& meow_show(const T& v) { return v; }\n\n"
         "int main() {\n" +
         body + " return 0;\n }\n";
}

std::string CodeGen::cppTypes(const TypeRef &t) {
  if (!t.primitive.has_value())
    return std::string(t.text);
  const std::string s = std::string(getPrimitiveSurname(*t.primitive));

  // Unsigned Ints
  if (s == "u8")
    return "uint8_t";
  if (s == "u16")
    return "uint16_t";
  if (s == "u32")
    return "uint32_t";
  if (s == "u64")
    return "uint64_t";
  // Signed Ints
  if (s == "i8")
    return "int8_t";
  if (s == "i16")
    return "int16_t";
  if (s == "i32")
    return "int32_t";
  if (s == "i64")
    return "int64_t";
  // FPN
  if (s == "f32")
    return "float";
  if (s == "f64")
    return "double";
  if (s == "bool")
    return "bool";
  if (s == "string")
    return "std::string";

  return s;
}

std::string CodeGen::expression(const ASTNode *node) {
  return std::visit(
      [this](const auto &v) -> std::string {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, NumberNode>) {
          return number(v.value);
        } else if constexpr (std::is_same_v<T, StringNode>) {
          return "std::string(\"" + escape(v.value) + "\")";
        } else if constexpr (std::is_same_v<T, BoolNode>) {
          return v.value ? "true" : "false";
        } else if constexpr (std::is_same_v<T, IdentifierNode>) {
          return v.name;
        } else if constexpr (std::is_same_v<T, UnaryExprNode>) {
          return "(" + operatorText(v.op) + expression(v.operand.get()) + ")";
        } else if constexpr (std::is_same_v<T, BinaryExprNode>) {
          return "(" + expression(v.left.get()) + " " + operatorText(v.op) +
                 " " + expression(v.right.get()) + ")";
        } else {
          throw std::runtime_error("Invalid Expression!");
        }
      },
      node->value);
}

std::string CodeGen::statement(const ASTNode *node) {
  return std::visit(
      [this](const auto &v) -> std::string {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, DeclarationNode>) {
          std::string result = cppTypes(v.type) + " " + v.name;

          if (v.initializer)
            result += " = " + expression(v.initializer.get());

          return result + ";";
        } else if constexpr (std::is_same_v<T, AssignmentNode>) {
          return v.name + " = " + expression(v.value.get()) + ";";
        } else if constexpr (std::is_same_v<T, PrintNode>) {
          return "std::cout << meow_show(" + expression(v.content.get()) +
                 ") << '\\n';";
        } else if constexpr (std::is_same_v<T, BlockNode>) {
          std::string result = "{\n";

          for (const auto &statement : v.statements)
            result += this->statement(statement.get()) + "\n";

          return result + "}";
        } else {
          throw std::runtime_error("Invalid Statement!");
        }
      },
      node->value);
}

// Helper functions

std::string number(const std::variant<std::int64_t, double> &v) {
  return std::visit(
      [](const auto &value) -> std::string {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::int64_t>) {
          return std::to_string(value);
        } else {
          char buf[64];
          auto result = std::to_chars(buf, buf + sizeof(buf), value);

          std::string s(buf, result.ptr);

          if (s.find_first_of(".eE") == std::string::npos)
            s += ".0";

          return s;
        }
      },
      v);
}

std::string escape(const std::string &input) {
  std::string output;
  for (char c : input) {
    switch (c) {
    case '\\':
      output += "\\\\";
      break;
    case '"':
      output += "\\\"";
      break;
    case '\n':
      output += "\\n";
      break;
    case '\t':
      output += "\\t";
      break;
    case '\r':
      output += "\\r";
      break;
    default:
      output += c;
    }
  }

  return output;
}

std::string operatorText(TokenType op) {
  switch (op) {
  case TokenType::Plus:
    return "+";
  case TokenType::Minus:
    return "-";
  case TokenType::Star:
    return "*";
  case TokenType::Slash:
    return "/";
  default:
    throw std::runtime_error("Invalid Operator!");
  }
}
