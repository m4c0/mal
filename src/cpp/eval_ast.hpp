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

    type operator()(types::hashmap in) {
      types::hashmap out;
      for (auto & v : in.take()) {
        auto nv = v.second.visit(EVAL { m_e });
        if (nv.is_error()) return nv;

        out = out + hashmap_entry<type> { v.first, std::move(nv) };
      }
      return std::move(out);
    }

    type operator()(types::list in) {
      types::list out;
      for (auto & v : in.take()) {
        auto nv = v.visit(EVAL { m_e });
        if (nv.is_error()) return nv;

        out = out + std::move(nv);
      }
      return std::move(out);
    }

    type operator()(types::vector in) {
      types::vector out;
      for (auto & v : in.take()) {
        auto nv = v.visit(EVAL { m_e });
        if (nv.is_error()) return nv;

        out = out + std::move(nv);
      }
      return std::move(out);
    }

    type operator()(types::symbol in) {
      return m_e->lookup(in);
    }

    type operator()(auto in) {
      return std::move(in);
    }
  };
}
