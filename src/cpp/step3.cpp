#include "core.hpp"
#include "env.hpp"
#include "eval.hpp"
#include "eval_ast.hpp"
#include "mal/list.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <string>

struct eval : public mal::eval::base<eval> {
  using base::base;
};

static auto READ(auto in) {
  return mal::read_str(in);
}
static mal::type EVAL(const mal::type & in, mal::env * e) {
  return in.visit(eval { e });
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(auto in) {
  mal::env e;
  mal::core::setup_step2_funcs(e);

  return PRINT(EVAL(READ(in), &e));
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
