#include "core.hpp"
#include "env.hpp"
#include "eval.hpp"
#include "eval_ast.hpp"
#include "mal/list.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <string>

static auto READ(auto in) {
  return mal::read_str(in);
}
static mal::type EVAL(const mal::type & in, auto e) {
  return in.visit(mal::eval { e });
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(const std::string & in, std::shared_ptr<mal::env> e) {
  return PRINT(EVAL(READ(in), std::move(e)));
}

static auto readline(std::string & line) {
  std::cout << "user> ";

  return static_cast<bool>(std::getline(std::cin, line));
}
int main() {
  auto e = std::make_shared<mal::env>();
  mal::core::setup_step4_funcs(rep, e);

  std::string line;
  while (readline(line)) {
    std::cout << rep(line, e) << "\n";
  }
}
