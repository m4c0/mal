#pragma once

#include "env.hpp"
#include "eval_ast.hpp"
#include "types.hpp"

namespace mal {
  class eval {
    env * m_e;

    [[nodiscard]] type def(const types::list & in) const noexcept {
      const auto & list = (*in).peek();
      if (list.size() != 3) return types::error { "def! must have a symbol and a value" };

      auto key = list[1].to_symbol();
      if (key.empty()) return types::error { "def! can only be called for symbols" };

      auto value = list[2].visit(*this);
      if (!value.is_error()) m_e->set(key, value);
      return value;
    }
    [[nodiscard]] auto let(const types::list & in) const noexcept {
      return in;
    }

  public:
    constexpr explicit eval(env * e) noexcept : m_e { e } {
    }

    type operator()(const types::list & in) const noexcept {
      if ((*in).begin() == (*in).end()) return in;

      auto first = *(*in).begin();
      if (first.to_symbol() == "def!") return def(in);
      if (first.to_symbol() == "let*") return let(in);

      auto evald = eval_ast<eval> { m_e }(in);
      if (evald.is_error()) return evald;

      const auto & list = (*evald.as<types::list>()).peek();

      auto oper = list.at(0).as<types::lambda>();
      auto args = std::span(list).subspan(1);
      return (*oper)(args);
    }

    type operator()(auto in) const noexcept {
      return eval_ast<eval> { m_e }(std::move(in));
    }
  };
}
