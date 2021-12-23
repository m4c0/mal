#pragma once

#include "env.hpp"

#include <concepts>
#include <type_traits>
#include <utility>

namespace mal {
  static mal::type EVAL(const mal::type & in, const std::shared_ptr<mal::env> & e);

  class eval_ast {
    std::shared_ptr<env> m_e;

  public:
    explicit eval_ast(std::shared_ptr<env> e) noexcept : m_e { std::move(e) } {
    }

    type operator()(const types::hashmap & in) {
      hashmap<type> out;
      for (const auto & v : *in) {
        auto nv = EVAL(v.second, m_e);
        if (nv.is_error()) return nv;

        out = out + hashmap_entry<type> { v.first, std::move(nv) };
      }
      return types::hashmap { std::move(out) };
    }

    type operator()(const types::list & in) {
      list<type> out;
      for (const auto & v : *in) {
        auto nv = EVAL(v, m_e);
        if (nv.is_error()) return nv;

        out = out + std::move(nv);
      }
      return types::list { std::move(out) };
    }

    type operator()(const types::vector & in) {
      vector<type> out;
      for (const auto & v : *in) {
        auto nv = EVAL(v, m_e);
        if (nv.is_error()) return nv;

        out = out + std::move(nv);
      }
      return types::vector { std::move(out) };
    }

    type operator()(const types::symbol & in) {
      return m_e->lookup(in);
    }

    type operator()(const auto & in) {
      return in;
    }
  };
}
