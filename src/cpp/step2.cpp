#include "env.hpp"
#include "mal/list.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <variant>

template<typename Op>
[[nodiscard]] static mal::type oper(std::span<mal::type> args, Op && op) {
  if (args.size() < 2) return mal::types::error { "Operation requires at least two operands" };

  int i = mal::to_int(args[0]);
  int res = std::accumulate(args.begin() + 1, args.end(), i, [op](int a, const auto & b) noexcept {
    return op(a, mal::to_int(b));
  });
  return mal::types::number { res };
}
[[nodiscard]] static mal::type divides(std::span<mal::type> args) {
  return oper(args, std::divides<>());
}
[[nodiscard]] static mal::type minus(std::span<mal::type> args) {
  return oper(args, std::minus<>());
}
[[nodiscard]] static mal::type multiplies(std::span<mal::type> args) {
  return oper(args, std::multiplies<>());
}
[[nodiscard]] static mal::type plus(std::span<mal::type> args) {
  return oper(args, std::plus<>());
}

static mal::type EVAL(mal::type in, mal::env * e);

class eval_ast {
  mal::env * m_e;

public:
  constexpr explicit eval_ast(mal::env * e) noexcept : m_e { e } {
  }

  mal::type operator()(mal::types::hashmap in) {
    mal::types::hashmap out;
    for (auto & v : in.take()) {
      auto nv = EVAL(std::move(v.second), m_e);
      if (std::holds_alternative<mal::types::error>(nv)) return nv;

      out = out + mal::hashmap_entry<mal::type> { v.first, std::move(nv) };
    }
    return std::move(out);
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

  mal::type operator()(mal::types::vector in) {
    mal::types::vector out;
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
  mal::env * m_e;

public:
  constexpr explicit eval(mal::env * e) noexcept : m_e { e } {
  }

  mal::type operator()(mal::types::list in) {
    if (in.begin() == in.end()) return std::move(in);

    auto evald = eval_ast { m_e }(std::move(in));
    if (std::holds_alternative<mal::types::error>(evald)) return evald;

    auto list = std::get<mal::types::list>(evald).take();

    auto oper = std::get<mal::types::lambda>(list.at(0));
    auto args = std::span(list).subspan<1>();
    return (*oper)(args);
  }
  mal::type operator()(auto in) {
    return eval_ast { m_e }(std::move(in));
  }
};

static auto READ(auto in) {
  return mal::read_str(in);
}
static mal::type EVAL(mal::type in, mal::env * e) {
  return std::visit(eval { e }, std::move(in));
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(auto in) {
  mal::env e;
  e.emplace("+", mal::types::lambda { plus });
  e.emplace("-", mal::types::lambda { minus });
  e.emplace("*", mal::types::lambda { multiplies });
  e.emplace("/", mal::types::lambda { divides });

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
