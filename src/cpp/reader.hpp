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
    type operator()(list<type> l) const noexcept {
      return { types::list { std::move(l) } };
    }
    type operator()(str n) const noexcept {
      return { types::string { std::move(n) } };
    }
    type operator()(vector<type> l) const noexcept {
      return { types::vector { std::move(l) } };
    }

    type operator()(auto n) const noexcept {
      return { std::move(n) };
    }
  };

  static auto read_str(const std::string & line) {
    auto res = parser::read_str(line, reader {});
    return res ? std::move(*res) : types::error {};
  }
}
