#pragma once

#include "env.hpp"
#include "eval.fn.hpp"
#include "eval.hpp"
#include "eval_ast.hpp"
#include "types.hpp"

#include <utility>

namespace mal::impl {
  class eval {
    using iteration = types::details::lambda_ret_t;

    [[nodiscard]] static iteration err(const std::string & msg) noexcept {
      return iteration { {}, types::error { msg } };
    }

    const std::shared_ptr<env> m_e;

    [[nodiscard]] iteration def(const types::list & in) const noexcept {
      const auto & list = *in;
      if (list.size() != 3) return err("def! must have a symbol and a value");

      auto key = list[1].to_symbol();
      if (key.empty()) return err("def! can only be called for symbols");

      log::debug() << "def! " << m_e.get() << " " << key << "\n";

      auto value = EVAL(list[2], m_e);
      if (!value.is_error()) m_e->set(key, value);
      return { {}, value };
    }
    [[nodiscard]] iteration let(const types::list & in) noexcept {
      auto inner = std::make_shared<env>(m_e);

      const auto & list = *in;
      if (list.size() != 3) return err("let* must have an env and an expression");

      auto e = list[1].to_iterable();

      if (e.size() % 2 == 1) return err("let* env must have a balanced list of key and values");

      for (int i = 0; i < e.size(); i += 2) {
        auto key = e[i].to_symbol();
        if (key.empty()) return err("let* env can only have symbol as keys");

        auto value = EVAL(e[i + 1], inner);
        if (value.is_error()) return { {}, value };
        inner->set(key, value);
      }

      return { inner, list[2] };
    }
    [[nodiscard]] iteration do_(const types::list & in) const noexcept {
      const auto & list = *in;
      for (auto it = list.begin() + 1; it != list.end() - 1; ++it) {
        auto r = EVAL(*it, m_e);
        if (r.is_error()) return { {}, r };
      }
      return { m_e, list.back() };
    }
    [[nodiscard]] iteration if_(const types::list & in) const noexcept {
      const auto & list = *in;
      if (list.size() < 3 || list.size() > 4) {
        return err("if must have condition, true and optionally false");
      }

      auto res = EVAL(list[1], m_e);
      if (res.is_error()) return { {}, res };

      if (res.to_boolean()) {
        return { m_e, list[2] };
      }
      if (list.size() == 3) {
        return { {}, types::nil {} };
      }
      return { m_e, list[3] };
    }

    [[nodiscard]] static iteration quote(const types::list & in) noexcept {
      const auto & list = *in;
      if (list.size() != 2) return err("quote requires a parameter");
      return { {}, list[1] };
    }

  public:
    explicit eval(std::shared_ptr<env> e) noexcept : m_e { std::move(e) } {
    }

    iteration operator()(const types::list & in) noexcept {
      if ((*in).begin() == (*in).end()) return { {}, in };

      auto first = (*in).begin()->to_symbol();
      if (first == "def!") return def(in);
      if (first == "let*") return let(in);
      if (first == "do") return do_(in);
      if (first == "if") return if_(in);
      if (first == "fn*") return { {}, evals::fn(m_e, in) };
      if (first == "quote") return quote(in);

      auto evald = eval_ast { m_e }(in);
      if (evald.is_error()) return { {}, evald };

      const auto & list = *evald.as<types::list>();

      auto oper = list.at(0).as<types::lambda>();
      auto args = std::span(list).subspan(1);
      return (*oper)(args, m_e);
    }

    iteration operator()(const auto & in) const noexcept {
      return { {}, eval_ast { m_e }(in) };
    }
  };

}
namespace mal {
  static type EVAL(const type & in, std::shared_ptr<env> e) {
    type var = in;
    while (e) {
      auto iter = var.visit(impl::eval { e });
      e = iter.e;
      var = iter.t;
    }
    return var;
  }
}
