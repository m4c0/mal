#pragma once

#include "mal/reader.hpp"
#include "mal/str.hpp"

#include <functional>
#include <span>
#include <utility>
#include <variant>

namespace mal::types {
  class type;

  class boolean {
    bool m_v;

  public:
    constexpr explicit boolean(bool v) noexcept : m_v(v) {
    }

    [[nodiscard]] constexpr bool operator*() const noexcept {
      return m_v;
    }
  };
  class number {
    int m_n;

  public:
    constexpr explicit number(int n) noexcept : m_n(n) {
    }

    [[nodiscard]] constexpr int operator*() const noexcept {
      return m_n;
    }
  };

  class error {
    std::string m_msg;

  public:
    error() : m_msg { "EOF" } {
    }
    explicit error(std::string m) : m_msg { std::move(m) } {
    }

    [[nodiscard]] constexpr const auto & message() const noexcept {
      return m_msg;
    }
  };

  class lambda {
    std::function<type(std::span<type>)> m_fn;

  public:
    explicit lambda(decltype(m_fn) f) : m_fn(std::move(f)) {
    }

    [[nodiscard]] constexpr auto & operator*() noexcept {
      return m_fn;
    }
  };

  using keyword = parser::token<parser::kw>;
  using nil = parser::nil;
  using string = str;
  using symbol = parser::token<void>;

  using hashmap = mal::hashmap<type>;
  using list = mal::list<type>;
  using vector = mal::vector<type>;

  class type
    : public std::variant<error, boolean, hashmap, number, keyword, lambda, list, nil, string, symbol, vector> {
  public:
    using variant::variant;

    [[nodiscard]] constexpr bool is_error() const noexcept {
      return std::holds_alternative<error>(*this);
    }

    template<typename Visitor>
    [[nodiscard]] auto visit(Visitor && v) noexcept {
      return std::visit(std::forward<Visitor>(v), std::move(*this));
    }
  };
}
namespace mal {
  using type = types::type;

  [[nodiscard]] static int to_int(const type & a) noexcept {
    if (const auto * v = std::get_if<types::number>(&a)) {
      return **v;
    }
    return 0;
  }
}
