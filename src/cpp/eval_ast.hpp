#pragma once

#include "env.hpp"

#include <concepts>
#include <type_traits>
#include <variant>

namespace mal {
  template<typename E>
  concept can_eval = requires(E e, mal::type t) {
    { std::visit(e, std::move(t)) } -> std::same_as<type>;
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
        auto nv = std::visit(EVAL { m_e }, std::move(v.second));
        if (std::holds_alternative<types::error>(nv)) return nv;

        out = out + hashmap_entry<type> { v.first, std::move(nv) };
      }
      return std::move(out);
    }

    type operator()(types::list in) {
      types::list out;
      for (auto & v : in.take()) {
        auto nv = std::visit(EVAL { m_e }, std::move(v));
        if (std::holds_alternative<types::error>(nv)) return nv;

        out = out + std::move(nv);
      }
      return std::move(out);
    }

    type operator()(types::vector in) {
      types::vector out;
      for (auto & v : in.take()) {
        auto nv = std::visit(EVAL { m_e }, std::move(v));
        if (std::holds_alternative<types::error>(nv)) return nv;

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
