#include <cstdint>
#include <iostream>
#include <string>
inline int meow_show(std::uint8_t v) { return v; }
inline int meow_show(std::uint16_t v) { return v; }
template<class T> const T& meow_show(const T& v) { return v; }

int main() {
 std::string x = std::string("20");
 return 0;
 }
