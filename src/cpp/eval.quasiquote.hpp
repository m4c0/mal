#pragma once

#include "log.hpp"
#include "printer.hpp"
#include "types.hpp"

#include <vector>

namespace mal::evals::impl {
  [[nodiscard]] static type quasiquote(const type & ast) noexcept {
    if (ast.is<types::hashmap>() || ast.is<types::symbol>()) {
      const type quote = types::symbol { "quote" };
      return types::list { quote, ast };
    }
    if (!ast.is_iterable()) {
      return ast;
    }

    bool ast_is_vec = ast.is<types::vector>();
    auto ast_items = ast.to_iterable();
    if (!ast_is_vec && ast_items.size() == 2 && ast_items[0].to_symbol() == "unquote") {
      return ast_items[1];
    }

    const type concat = types::symbol { "concat" };
    const type cons = types::symbol { "cons" };

    type res = types::list { std::vector<type> {} };
    // NOLINTNEXTLINE since ranges are not available in my setup
    for (auto elt_it = ast_items.rbegin(); elt_it != ast_items.rend(); ++elt_it) {
      const auto & elt = *elt_it;
      if (elt.is_iterable()) {
        auto elt_items = elt.to_iterable();
        if (elt_items.size() == 2 && elt_items[0].to_symbol() == "splice-unquote") {
          res = types::list { concat, elt_items[1], res };
          continue;
        }
      }
      res = types::list { cons, quasiquote(elt), res };
    }

    if (ast_is_vec) {
      const type vec = types::symbol { "vec" };
      return types::list { vec, res };
    }
    return res;
  }
}
namespace mal::evals {
  [[nodiscard]] static type quasiquote(const types::list & in) noexcept {
    std::span<const type> list = *in;
    if (list.size() != 2) return err("quasiquote requires a single argument");
    return impl::quasiquote(in.at(1));
  }
}
