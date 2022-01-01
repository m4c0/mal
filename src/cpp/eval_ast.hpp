#pragma once

#include "env.hpp"
#include "eval.hpp"

#include <concepts>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace mal {
  class eval_ast {
    template<typename Tp>
    static void apply_on_iterable(type * v, senv e) {
      const auto & orig = *v->as<Tp>();

      std::vector<type> items;
      items.reserve(orig.size());
      for (int i = 0; i < orig.size(); i++) {
        items.emplace_back(EVAL(orig[i], e));
        const auto & item = items.back();
        if (item.is_error()) {
          *v = item;
          [[unlikely]] return;
        }
      }
      *v = Tp { std::move(items) };
    }

    static void apply_on_hashmap(type * v, senv e) {
      auto map = *v->as<types::hashmap>();
      for (auto & [key, val] : map) {
        val = EVAL(val, e);
        if (val.is_error()) {
          *v = val;
          [[unlikely]] return;
        }
      }
      *v = types::hashmap { std::move(map) };
    }

  public:
    static void visit(type * v, senv e) noexcept {
      if (v->is<types::symbol>()) {
        *v = e->lookup(v->to_symbol());
        return;
      }
      if (v->is<types::hashmap>()) {
        apply_on_hashmap(v, e);
        return;
      }
      if (v->is<types::list>()) {
        apply_on_iterable<types::list>(v, e);
        return;
      }
      if (v->is<types::vector>()) {
        apply_on_iterable<types::vector>(v, e);
        return;
      }
    }
  };
}
