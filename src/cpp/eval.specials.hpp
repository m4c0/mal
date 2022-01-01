#pragma once

#include "env.hpp"
#include "eval.common.hpp"
#include "eval.hpp"

namespace mal::evals {
  [[nodiscard]] static iteration def(const types::list & in, senv m_e) noexcept {
    const auto & list = *in;
    if (list.size() != 3) return err_i("def! must have a symbol and a value");

    auto key = list[1].to_symbol();
    if (key.empty()) return err_i("def! can only be called for symbols");

    log::debug() << "def! " << m_e.get() << " " << key << "\n";

    auto value = EVAL(list[2], m_e);
    if (!value.is_error()) m_e->set(key, value);
    return { {}, value };
  }

  [[nodiscard]] static iteration let(const types::list & in, senv m_e) noexcept {
    auto inner = m_e->extend();

    const auto & list = *in;
    if (list.size() != 3) return err_i("let* must have an env and an expression");

    auto e = list[1].to_iterable();

    if (e.size() % 2 == 1) return err_i("let* env must have a balanced list of key and values");

    for (int i = 0; i < e.size(); i += 2) {
      auto key = e[i].to_symbol();
      if (key.empty()) return err_i("let* env can only have symbol as keys");

      auto value = EVAL(e[i + 1], inner);
      if (value.is_error()) return { {}, value };
      inner->set(key, value);
    }

    return { inner, list[2] };
  }

  [[nodiscard]] static iteration do_(const types::list & in, senv m_e) noexcept {
    const auto & list = *in;
    for (auto it = list.begin() + 1; it != list.end() - 1; ++it) {
      auto r = EVAL(*it, m_e);
      if (r.is_error()) return { {}, r };
    }
    return { m_e, list.back() };
  }

  [[nodiscard]] static iteration if_(const types::list & in, senv m_e) noexcept {
    const auto & list = *in;
    if (list.size() < 3 || list.size() > 4) {
      return err_i("if must have condition, true and optionally false");
    }

    auto res = EVAL(list[1], m_e);
    if (res.is_error()) return { {}, res };

    if (!res.is<types::nil>() && res.to_boolean(true)) {
      return { m_e, list[2] };
    }
    if (list.size() == 3) {
      return { {}, types::nil {} };
    }
    return { m_e, list[3] };
  }
}
