#include "core7.hpp"
#include "env.hpp"
#include "eval.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <sstream>
#include <string>

static auto READ(auto in) {
  return mal::read_str(in);
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(const std::string & in, const std::shared_ptr<mal::env> & e) {
  return PRINT(EVAL(READ(in), e));
}

static auto readline(std::string & line) {
  std::cout << "user> ";

  return static_cast<bool>(std::getline(std::cin, line));
}
int main(int argc, char ** argv) {
  auto e = std::make_shared<mal::env>();
  mal::core::setup_step7_funcs(rep, e);
  if (mal::core::setup_argv(argc, argv, rep, e)) {
    return 0;
  }

  std::string line;
  while (readline(line)) {
    std::cout << rep(line, e) << "\n";
  }
}
