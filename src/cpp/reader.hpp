#pragma once

#include "mal/reader.hpp"
#include "mal/str.hpp"
#include "types.hpp"

#include <functional>
#include <span>
#include <utility>
#include <variant>

namespace mal {
  class reader {
  public:
    type operator()(bool b) const noexcept {
      return { types::boolean { b } };
    }
    type operator()(hashmap<type> l) const noexcept {
      return { types::hashmap { std::move(l) } };
    }
    type operator()(int n) const noexcept {
      return { types::number { n } };
    }
    type operator()(parser::nil /*n*/) const noexcept {
      return nullptr;
    }
    type operator()(list<type> l) const noexcept {
      return { types::list { std::move(l) } };
    }
    type operator()(parser::token<parser::kw> l) const noexcept {
      return { types::keyword { l } };
    }
    type operator()(parser::token<void> l) const noexcept {
      return { types::symbol { l } };
    }
    type operator()(str n) const noexcept {
      return { types::string { *n } };
    }
    type operator()(vector<type> l) const noexcept {
      return { types::vector { std::move(l) } };
    }
  };

  static auto read_str(const std::string & line) {
    auto res = parser::read_str(line, reader {});
    return res ? std::move(*res) : types::error { "EOF" };
  }
}
