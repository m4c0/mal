#include "printer.hpp"
#include "reader.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <variant>

class env {
public:
  [[nodiscard]] mal::type lookup(mal::types::symbol s) const noexcept {
    return mal::types::error { "Symbol not found" };
  }
};

static mal::type EVAL(mal::type in, env * e);

class eval_ast {
  env * m_e;

public:
  constexpr explicit eval_ast(env * e) noexcept : m_e { e } {
  }

  mal::type operator()(mal::types::list in) {
    mal::types::list out;
    for (auto & v : in.take()) {
      auto nv = EVAL(std::move(v), m_e);
      if (std::holds_alternative<mal::types::error>(nv)) return nv;

      out = out + std::move(nv);
    }
    return std::move(out);
  }
  mal::type operator()(mal::types::symbol in) {
    return m_e->lookup(in);
  }
  mal::type operator()(auto in) {
    return std::move(in);
  }
};
class eval {
  env * m_e;

public:
  constexpr explicit eval(env * e) noexcept : m_e { e } {
  }

  mal::type operator()(mal::types::list in) {
    if (in.begin() == in.end()) return std::move(in);

    auto evald = eval_ast { m_e }(std::move(in));
    if (std::holds_alternative<mal::types::error>(evald)) return evald;

    auto list = std::get<mal::types::list>(evald).take();

    auto oper = std::get<mal::types::error>(list.at(0));

    return std::move(mal::types::error {});
  }
  mal::type operator()(auto in) {
    return eval_ast { m_e }(std::move(in));
  }
};

static auto READ(auto in) {
  return mal::read_str(in);
}
static mal::type EVAL(mal::type in, env * e) {
  return std::visit(eval { e }, std::move(in));
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(auto in) {
  env e;
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
