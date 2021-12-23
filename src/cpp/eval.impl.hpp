#pragma once

#include "env.hpp"
#include "eval.fn.hpp"
#include "eval.hpp"
#include "eval_ast.hpp"
#include "types.hpp"

#include <utility>

namespace mal::impl {
  class eval {
    struct iteration {
      bool tco {};
      type res {};
    };
    [[nodiscard]] static iteration err(const std::string & msg) noexcept {
      return iteration { false, types::error { msg } };
    }

    std::shared_ptr<env> m_e;

    [[nodiscard]] iteration def(const types::list & in) const noexcept {
      const auto & list = (*in).peek();
      if (list.size() != 3) return err("def! must have a symbol and a value");

      auto key = list[1].to_symbol();
      if (key.empty()) return err("def! can only be called for symbols");

      auto value = EVAL(list[2], m_e);
      if (!value.is_error()) m_e->set(key, value);
      return { false, value };
    }
    [[nodiscard]] iteration let(const types::list & in) noexcept {
      auto inner = std::make_shared<env>(m_e);

      const auto & list = (*in).peek();
      if (list.size() != 3) return err("let* must have an env and an expression");

      const std::vector<type> * e {};
      if (list[1].is<types::list>()) e = &(*list[1].as<types::list>()).peek();
      if (list[1].is<types::vector>()) e = &(*list[1].as<types::vector>()).peek();
      if (e == nullptr) return err("let* env must be a list");

      if (e->size() % 2 == 1) return err("let* env must have a balanced list of key and values");

      for (int i = 0; i < e->size(); i += 2) {
        auto key = e->at(i).to_symbol();
        if (key.empty()) return err("let* env can only have symbol as keys");

        auto value = EVAL(e->at(i + 1), inner);
        if (value.is_error()) return { false, value };
        inner->set(key, value);
      }

      m_e = inner;
      return { true, list[2] };
    }
    [[nodiscard]] iteration do_(const types::list & in) const noexcept {
      const auto & list = (*in).peek();
      for (auto it = list.begin() + 1; it != list.end() - 1; ++it) {
        auto r = EVAL(*it, m_e);
        if (r.is_error()) return { false, r };
      }
      return { true, list.back() };
    }
    [[nodiscard]] iteration if_(const types::list & in) const noexcept {
      const auto & list = (*in).peek();
      if (list.size() < 3 || list.size() > 4) {
        return err("if must have condition, true and optionally false");
      }

      auto res = EVAL(list[1], m_e);
      if (res.is_error()) return { false, res };

      if (res.to_boolean()) {
        return { true, list[2] };
      }
      if (list.size() == 3) {
        return { false, types::nil {} };
      }
      return { true, list[3] };
    }

  public:
    explicit eval(std::shared_ptr<env> e) noexcept : m_e { std::move(e) } {
    }

    iteration operator()(const types::list & in) noexcept {
      if ((*in).begin() == (*in).end()) return { false, in };

      auto first = (*in).begin()->to_symbol();
      if (first == "def!") return def(in);
      if (first == "let*") return let(in);
      if (first == "do") return do_(in);
      if (first == "if") return if_(in);
      if (first == "fn*") return { false, evals::fn<eval>(m_e, in) };

      auto evald = eval_ast { m_e }(in);
      if (evald.is_error()) return { false, evald };

      const auto & list = (*evald.as<types::list>()).peek();

      auto oper = list.at(0).as<types::lambda>();
      auto args = std::span(list).subspan(1);
      return { false, *(*oper)(args) };
    }

    iteration operator()(const auto & in) const noexcept {
      return { false, eval_ast { m_e }(in) };
    }
  };

}
namespace mal {
  static type EVAL(const type & in, const std::shared_ptr<mal::env> & e) {
    type var = in;
    impl::eval eva { e };
    while (true) {
      auto iter = var.visit(eva);
      if (!iter.tco) return iter.res;
      var = iter.res;
    }
  }
}
