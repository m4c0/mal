#pragma once

#include "mal/reader.hpp"
#include "mal/str.hpp"

#include <functional>
#include <span>
#include <utility>
#include <variant>

namespace mal::types::details {
  template<typename Tp>
  class holder {
    Tp m_v;

  public:
    constexpr explicit holder(Tp v) noexcept : m_v(std::move(v)) {
    }

    [[nodiscard]] constexpr const Tp & operator*() const noexcept {
      return m_v;
    }
  };
}
namespace mal::types {
  class type;

  struct error : details::holder<std::string> {
    using holder::holder;

    error() : holder("EOF") {
    }
  };

  using boolean = details::holder<bool>;
  using keyword = parser::token<parser::kw>;
  using lambda = details::holder<std::function<type(std::span<type>)>>;
  using nil = parser::nil;
  using number = details::holder<int>;
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
