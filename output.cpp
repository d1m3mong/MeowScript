#include <cstdint>
#include <iostream>
#include <string>
inline int meow_show(std::uint8_t v) { return v; }
inline int meow_show(std::uint16_t v) { return v; }
template<class T> const T& meow_show(const T& v) { return v; }

int main() {
 uint64_t catnip = 100;
 float price = 9.99;
 std::string name = std::string("Meow");
 int32_t total = ((1 + 2) * 4);
 std::cout << meow_show(total) << '\n';
 std::cout << meow_show(name) << '\n';
 std::cout << meow_show(price) << '\n';
 return 0;
 }
