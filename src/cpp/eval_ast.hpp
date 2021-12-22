#pragma once

#include "env.hpp"

#include <concepts>
#include <type_traits>

namespace mal {
  template<typename E>
  concept can_eval = requires(E e, mal::type t) {
    { t.visit(e) } -> std::same_as<type>;
  };

  template<can_eval EVAL>
  class eval_ast {
    env * m_e;

  public:
    constexpr explicit eval_ast(env * e) noexcept : m_e { e } {
    }

    type operator()(const types::hashmap & in) {
      hashmap<type> out;
      for (const auto & v : *in) {
        auto nv = v.second.visit(EVAL { m_e });
        if (nv.is_error()) return nv;

        out = out + hashmap_entry<type> { v.first, std::move(nv) };
      }
      return types::hashmap { std::move(out) };
    }

    type operator()(const types::list & in) {
      list<type> out;
      for (const auto & v : *in) {
        auto nv = v.visit(EVAL { m_e });
        if (nv.is_error()) return nv;

        out = out + std::move(nv);
      }
      return types::list { std::move(out) };
    }

    type operator()(const types::vector & in) {
      vector<type> out;
      for (const auto & v : *in) {
        auto nv = v.visit(EVAL { m_e });
        if (nv.is_error()) return nv;

        out = out + std::move(nv);
      }
      return types::vector { std::move(out) };
    }

    type operator()(types::symbol in) {
      return m_e->lookup(in);
    }

    type operator()(auto in) {
      return std::move(in);
    }
  };
}
