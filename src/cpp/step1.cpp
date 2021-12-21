#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <string>

static auto READ(auto in) {
  return mal::read_str(in);
}
static auto EVAL(auto in) {
  return in;
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(auto in) {
  return PRINT(EVAL(READ(in)));
}

static auto readline(std::string & line) {
  std::cout << "user> ";

  return static_cast<bool>(std::getline(std::cin, line));
}
int main() {
  std::string line;
  while (readline(line)) {
    std::cout << rep(line) << "\n";
  }
}
