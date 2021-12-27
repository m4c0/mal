#pragma once

#include "types.hpp"

#include <vector>

namespace mal::evals::impl {
  [[nodiscard]] static type quasiquote(const type & ast) noexcept {
    if (!ast.is_iterable()) {
      const type quote = types::symbol { "quote" };
      return types::list { quote, ast };
    }

    auto ast_items = ast.to_iterable();
    if (ast_items.size() == 2 && ast_items[0].to_symbol() == "unquote") {
      return ast_items[1];
    }

    const type concat = types::symbol { "concat" };
    const type cons = types::symbol { "cons" };

    type res = types::list { std::vector<type> {} };
    for (const auto & elt : ast_items) {
      if (elt.is_iterable()) {
        auto elt_items = elt.to_iterable();
        if (elt_items.size() == 2 && elt_items[0].to_symbol() == "splice-unquote") {
          res = types::list { concat, elt_items[1], res };
        } else {
          res = types::list { cons, quasiquote(elt), res };
        }
      }
    }
    return res;
  }
}
namespace mal::evals {
  [[nodiscard]] static type quasiquote(const types::list & in) noexcept {
    std::span<const type> list = *in;
    if (list.size() != 2) return types::error { "quasiquote requires a single argument" };
    return impl::quasiquote(in.at(1));
  }
}
