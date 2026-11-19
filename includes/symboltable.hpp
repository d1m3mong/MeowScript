#pragma once

#include "types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct Symbol {
  TypeRef type;
  std::string name;
};

class SymbolTable {
  std::vector<std::unordered_map<std::string, Symbol>> scopes;

public:
  SymbolTable();

  void declare(const Symbol &symbol);
  const Symbol *lookup(const std::string &name) const;
  bool contains(const std::string &name) const;
  void enterScope();
  void exitScope();
};
