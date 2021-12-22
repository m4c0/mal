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
    [[nodiscard]] type let(const types::list & in) const noexcept {
      env inner { m_e };

      const auto & list = (*in).peek();
      if (list.size() != 3) return types::error { "let* must have an env and an expression" };

      const std::vector<type> * e {};
      if (list[1].is<types::list>()) e = &(*list[1].as<types::list>()).peek();
      if (list[1].is<types::vector>()) e = &(*list[1].as<types::vector>()).peek();
      if (e == nullptr) return types::error { "let* env must be a list" };

      if (e->size() % 2 == 1) return types::error { "let* env must have a balanced list of key and values" };

      eval new_eval { &inner };
      for (int i = 0; i < e->size(); i += 2) {
        auto key = e->at(i).to_symbol();
        if (key.empty()) return types::error { "let* env can only have symbol as keys" };

        auto value = e->at(i + 1).visit(new_eval);
        if (value.is_error()) return value;
        inner.set(key, value);
      }

      return list[2].visit(new_eval);
    }
    [[nodiscard]] type doit(const types::list & in) const noexcept {
      const auto & list = (*in).peek();
      for (auto it = list.begin() + 1; it != list.end() - 1; ++it) {
        auto r = it->visit(*this);
        if (r.is_error()) return r;
      }
      return list.back().visit(*this);
    }

  public:
    constexpr explicit eval(env * e) noexcept : m_e { e } {
    }

    type operator()(const types::list & in) const noexcept {
      if ((*in).begin() == (*in).end()) return in;

      auto first = (*in).begin()->to_symbol();
      if (first == "def!") return def(in);
      if (first == "let*") return let(in);
      if (first == "do") return doit(in);

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
