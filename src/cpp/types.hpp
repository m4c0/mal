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

  template<typename Tp>
  class heavy_holder {
    std::shared_ptr<Tp> m_v;

  public:
    explicit heavy_holder(Tp v) : m_v { std::make_shared<Tp>(std::move(v)) } {
    }

    [[nodiscard]] constexpr const Tp & operator*() const noexcept {
      return *m_v;
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
  using hashmap = details::heavy_holder<mal::hashmap<type>>;
  using keyword = parser::token<parser::kw>;
  using lambda = details::holder<std::function<type(std::span<const type>)>>;
  using list = details::heavy_holder<mal::list<type>>;
  using nil = parser::nil;
  using number = details::holder<int>;
  using string = details::heavy_holder<str>;
  using symbol = parser::token<void>;
  using vector = details::heavy_holder<mal::vector<type>>;

  class type {
    std::variant<error, boolean, hashmap, number, keyword, lambda, list, nil, string, symbol, vector> m_value {};

  public:
    constexpr type() = default;
    type(auto v) : m_value(std::move(v)) { // NOLINT - we want the simplicity of implicit
    }

    template<typename Tp>
    [[nodiscard]] const Tp & as() const noexcept {
      return std::get<Tp>(m_value);
    }

    template<typename Tp>
    [[nodiscard]] bool is() const noexcept {
      return std::holds_alternative<Tp>(m_value);
    }

    [[nodiscard]] constexpr bool is_error() const noexcept {
      return std::holds_alternative<error>(m_value);
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
    [[nodiscard]] std::string to_symbol() const noexcept {
      if (const auto * v = std::get_if<symbol>(&m_value)) {
        return { (*v).value.begin(), (*v).value.end() };
      }
      return "";
    }
  };
}
namespace mal {
  using type = types::type;

}
