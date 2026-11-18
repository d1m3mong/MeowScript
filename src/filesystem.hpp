#pragma once

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

class FileSystem {
  std::string content;

public:
  FileSystem(const char *source = "input.meow") {
    std::ifstream file(source);
    if (!file) {
      throw std::runtime_error(std::string("Could not open File: ") + source +
                               "!");
    }
    content.assign(std::istreambuf_iterator<char>(file),
                   std::istreambuf_iterator<char>());
  }

  std::string_view view() const { return content; }
};
