#include <cstdint>
#include <iostream>
#include <string>
inline int meow_show(std::uint8_t v) { return v; }
inline int meow_show(std::uint16_t v) { return v; }
template<class T> const T& meow_show(const T& v) { return v; }

int main() {
 int64_t x = 5;
 {
int64_t x = 10;
std::cout << meow_show(x) << '\n';
}
 std::cout << meow_show(x) << '\n';
 return 0;
 }
