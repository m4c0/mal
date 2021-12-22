#include "core.hpp"
#include "env.hpp"
#include "eval_ast.hpp"
#include "mal/list.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <string>

class eval {
  mal::env * m_e;

public:
  constexpr explicit eval(mal::env * e) noexcept : m_e { e } {
  }

  mal::type operator()(mal::types::list in) {
    if (in.begin() == in.end()) return std::move(in);

    auto evald = mal::eval_ast<eval> { m_e }(std::move(in));
    if (evald.is_error()) return evald;

    auto list = std::get<mal::types::list>(evald).take();

    auto oper = std::get<mal::types::lambda>(list.at(0));
    auto args = std::span(list).subspan<1>();
    return (*oper)(args);
  }

  mal::type operator()(auto in) {
    return mal::eval_ast<eval> { m_e }(std::move(in));
  }
};

static auto READ(auto in) {
  return mal::read_str(in);
}
static mal::type EVAL(mal::type in, mal::env * e) {
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
