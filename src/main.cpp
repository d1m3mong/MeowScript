#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "../includes/lexer.hpp"
#include "../includes/parser.hpp"
#include "filesystem.hpp"

int main(int argc, char **argv) {
  if (argc < 3 || argc > 4) {
    std::cerr << "Invalid amount of arguments!\n";
    return -1;
  }

  std::string source{argv[1]};
  if (source.empty() || source.size() < 6)
    std::cerr << "Source is empty!\n";
  if (source.substr(source.size() - 5, source.size()) != ".meow")
    std::cerr << "Source is not a .meow File!\n";

  try {
    FileSystem fsystem(source.c_str());

    std::string_view content = fsystem.view();

    Lexer lexer(content);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    if (!parser.parse()) {
      std::cerr << "Parse failed!\n";
    }

  } catch (const std::exception &except) {
    std::cerr << "Trapped: " << except.what() << '\n';
    return -1;
  }

  return 0;
}
