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

  class type {
    std::variant<error, boolean, hashmap, number, keyword, lambda, list, nil, string, symbol, vector> m_value {};

  public:
    constexpr type() = default;
    type(auto v) : m_value(std::move(v)) { // NOLINT - we want the simplicity of implicit
    }

    template<typename Tp>
    [[nodiscard]] Tp as() noexcept {
      return std::move(std::get<Tp>(m_value));
    }

    [[nodiscard]] constexpr bool is_error() const noexcept {
      return std::holds_alternative<error>(m_value);
    }

    template<typename Visitor>
    [[nodiscard]] auto visit(Visitor && v) noexcept {
      return std::visit(std::forward<Visitor>(v), std::move(m_value));
    }
    template<typename Visitor>
    [[nodiscard]] auto visit(Visitor && v) const noexcept {
      return std::visit(std::forward<Visitor>(v), m_value);
    }

    [[nodiscard]] int to_int() const noexcept {
      if (const auto * v = std::get_if<number>(&m_value)) {
        return **v;
      }
      return 0;
    }
  };
}
namespace mal {
  using type = types::type;

}
