#pragma once

#include "env.hpp"
#include "eval.hpp"

#include <concepts>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace mal {
  class eval_ast {
    std::shared_ptr<env> m_e;

  public:
    explicit eval_ast(std::shared_ptr<env> e) noexcept : m_e { std::move(e) } {
    }

    type operator()(const types::hashmap & in) {
      std::unordered_map<std::string, type> out;
      for (const auto & v : *in) {
        auto nv = EVAL(v.second, m_e);
        if (nv.is_error()) return nv;
        out[v.first] = nv;
      }
      return types::hashmap { std::move(out) };
    }

    type operator()(const types::list & in) {
      std::vector<type> out;
      for (const auto & v : *in) {
        auto nv = EVAL(v, m_e);
        if (nv.is_error()) return nv;
        out.push_back(nv);
      }
      return types::list { out };
    }

    type operator()(const types::vector & in) {
      std::vector<type> out;
      for (const auto & v : *in) {
        auto nv = EVAL(v, m_e);
        if (nv.is_error()) return nv;
        out.push_back(std::move(nv));
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
