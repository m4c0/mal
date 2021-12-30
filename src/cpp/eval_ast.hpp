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
    static void apply_on_iterable(type * v, const std::shared_ptr<env> & e) {
      std::vector<type> items { *v->as<Tp>() };
      for (auto & item : items) {
        item = EVAL(item, e);
        if (item.is_error()) {
          *v = item;
          [[unlikely]] return;
        }
      }
      *v = Tp { items };
    }

    static void apply_on_hashmap(type * v, const std::shared_ptr<env> & e) {
      auto map = *v->as<types::hashmap>();
      for (auto & [key, val] : map) {
        val = EVAL(val, e);
        if (val.is_error()) {
          *v = val;
          [[unlikely]] return;
        }
      }
      *v = types::hashmap { map };
    }

  public:
    static void visit(type * v, const std::shared_ptr<env> & e) noexcept {
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
