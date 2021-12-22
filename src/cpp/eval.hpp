#pragma once

#include "env.hpp"
#include "eval_ast.hpp"
#include "types.hpp"

namespace mal::eval {
  template<typename Self>
  class base {
    env * m_e;

  protected:
    [[nodiscard]] virtual type eval_non_empty_list(const types::list & in) {
      auto evald = eval_ast<Self> { m_e }(in);
      if (evald.is_error()) return evald;

      const auto & list = (*evald.template as<types::list>()).peek();

      auto oper = list.at(0).template as<types::lambda>();
      auto args = std::span(list).subspan(1);
      return (*oper)(args);
    }

  public:
    constexpr explicit base(env * e) noexcept : m_e { e } {
    }

    type operator()(const types::list & in) {
      if ((*in).begin() == (*in).end()) return in;
      return this->eval_non_empty_list(in);
    }

    type operator()(auto in) {
      return eval_ast<Self> { m_e }(std::move(in));
    }
  };
}
