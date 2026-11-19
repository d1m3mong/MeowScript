#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "../includes/codegen.hpp"
#include "../includes/lexer.hpp"
#include "../includes/parser.hpp"
#include "../includes/semanticanalyzer.hpp"

#include "filesystem.hpp"

int main(int argc, char **argv) {
  if (argc != 4) {
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
      return -1;
    }

    SemanticAnalyzer semantic;

    semantic.analyze(parser.getAST());

    std::filesystem::path outputPath(argv[3]);

    std::filesystem::path cppFile = outputPath;
    cppFile.replace_extension(".cpp");

    CodeGen codegen;

    std::ofstream out(cppFile);
    if (!out) {
      std::cerr << "Could not create output file!\n";
      return 1;
    }

    out << codegen.generate(parser.getAST());
    out.close();

    if (!out) {
      std::cerr << "Could not write output file!\n";
      return 1;
    }

    std::string command = "g++ -std=c++20 \"" + cppFile.string() + "\" -o \"" +
                          outputPath.string() + "\"";

    int result = std::system(command.c_str());

    if (result != 0) {
      std::cerr << "Compilation failed!\n";
      return 1;
    }
  } catch (const std::exception &except) {
    std::cerr << "Trapped: " << except.what() << '\n';
    return -1;
  }

  return 0;
}
