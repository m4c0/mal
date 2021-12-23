#include "core.hpp"
#include "env.hpp"
#include "eval.impl.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <string>

static auto READ(auto in) {
  return mal::read_str(in);
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(auto in, auto e) {
  return PRINT(EVAL(READ(in), e));
}

static auto readline(std::string & line) {
  std::cout << "user> ";

  return static_cast<bool>(std::getline(std::cin, line));
}
int main() {
  auto e = std::make_shared<mal::env>();
  mal::core::setup_step2_funcs(e);

  std::string line;
  while (readline(line)) {
    std::cout << rep(line, e) << "\n";
  }
}
