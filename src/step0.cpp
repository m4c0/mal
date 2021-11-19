#include <iostream>
#include <string>

auto read(const std::string & s) {
  return s;
}
auto eval(const std::string & s) {
  return s;
}
auto print(const std::string & s) {
  return s;
}

auto rep(const std::string & s) {
  return print(eval(read(s)));
}

int main() {
  while (true) {
    std::cout << "user> ";

    for (std::string line; std::getline(std::cin, line);) {
      std::cout << rep(line) << "\nuser> ";
    }
  }
}
