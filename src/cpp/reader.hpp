#pragma once

#include "mal/reader.hpp"
#include "mal/str.hpp"

#include <utility>
#include <variant>

namespace mal::types {
  class type;

  class error {
    std::string m_msg;

  public:
    error() : m_msg { "OK" } {
    }
    explicit error(std::string m) : m_msg { std::move(m) } {
    }

    [[nodiscard]] constexpr const auto & message() const noexcept {
      return m_msg;
    }
  };
  using keyword = parser::token<parser::kw>;
  using nil = parser::nil;
  using string = str;
  using symbol = parser::token<void>;

  using hashmap = mal::hashmap<type>;
  using list = mal::list<type>;
  using vector = mal::vector<type>;

  class type : public std::variant<error, bool, hashmap, int, keyword, list, nil, string, symbol, vector> {
  public:
    using variant::variant;
  };
}
namespace mal {
  using type = types::type;

  class reader {
  public:
    type operator()(auto n) const noexcept {
      return { std::move(n) };
    }
  };

  static auto read_str(const std::string & line) {
    auto res = parser::read_str(line, reader {});
    return res ? std::move(*res) : types::error {};
  }
}
