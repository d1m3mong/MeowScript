#include "../includes/symboltable.hpp"
#include <stdexcept>

void SymbolTable::declare(const Symbol &symbol) {
  if (scopes.empty())
    throw std::runtime_error("No scope available!");

  auto &currentScope = scopes.back();

  if (currentScope.contains(symbol.name))
    throw std::runtime_error("Invalid declaration: " + symbol.name +
                             " already exists in current Scope!");

  currentScope.emplace(symbol.name, symbol.type);
}

bool SymbolTable::contains(const std::string &name) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->contains(name))
      return true;
  }
  return false;
}

SymbolTable::SymbolTable() { scopes.emplace_back(); }

const Symbol *SymbolTable::lookup(const std::string &name) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end())
      return &found->second;
  }

  return nullptr;
}

void SymbolTable::enterScope() { scopes.emplace_back(); }

void SymbolTable::exitScope() {
  if (scopes.size() > 1)
    scopes.pop_back();
}
