#pragma once

#include "mal/reader.hpp"
#include "mal/str.hpp"

#include <functional>
#include <span>
#include <utility>
#include <variant>

namespace mal {
  class env;
}
namespace mal::types {
  class type;
}
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

  template<typename Tp>
  class token_holder {
    std::string m_v;

  public:
    constexpr explicit token_holder(parser::token<Tp> v) noexcept : m_v { v.value.begin(), v.value.length() } {
    }

    [[nodiscard]] constexpr const std::string & operator*() const noexcept {
      return m_v;
    }
  };

  template<typename Tp>
  [[nodiscard]] static bool operator==(const holder<Tp> & a, const holder<Tp> & b) noexcept {
    return *a == *b;
  }
  template<typename Tp>
  [[nodiscard]] static bool operator==(const heavy_holder<Tp> & a, const heavy_holder<Tp> & b) noexcept {
    return *a == *b;
  }
  template<typename Tp>
  [[nodiscard]] static bool operator==(const token_holder<Tp> & a, const token_holder<Tp> & b) noexcept {
    return *a == *b;
  }

  struct lambda_ret_t;
  using lambda_args_t = std::span<const type>;
  using lambda_t = std::function<lambda_ret_t(lambda_args_t)>;
  [[nodiscard]] static lambda_t convert(std::function<type(lambda_args_t)> fn) noexcept;
}
namespace mal::types {
  struct error : details::holder<std::string> {
    using holder::holder;

    error() : holder("EOF") {
    }
  };

  struct lambda : details::heavy_holder<details::lambda_t> {
    explicit lambda(int /*marker*/, const details::lambda_t & fn) : heavy_holder { fn } {
    }
    explicit lambda(std::function<type(std::span<const type>)> fn) : heavy_holder { details::convert(std::move(fn)) } {
    }
  };

  using boolean = details::holder<bool>;
  using hashmap = details::heavy_holder<mal::hashmap<type>>;
  using keyword = details::token_holder<parser::kw>;
  using list = details::heavy_holder<mal::list<type>>;
  using nil = parser::nil;
  using number = details::holder<int>;
  using string = details::heavy_holder<str>;
  using symbol = details::token_holder<void>;
  using vector = details::heavy_holder<mal::vector<type>>;

  [[nodiscard]] static bool operator==(const lambda & /*a*/, const lambda & /*b*/) noexcept {
    return false;
  }

  class type {
    std::variant<error, boolean, hashmap, number, keyword, lambda, list, nil, string, symbol, vector> m_value {};

  public:
    constexpr type() = default;
    type(auto v) : m_value(std::move(v)) { // NOLINT - we want the simplicity of implicit
    }

    [[nodiscard]] bool operator==(const type & o) const noexcept {
      if (is<vector>() && o.is<list>()) return *as<vector>() == *o.as<list>();
      if (is<list>() && o.is<vector>()) return *as<list>() == *o.as<vector>();
      return m_value == o.m_value;
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

    [[nodiscard]] bool to_boolean() const noexcept {
      if (const auto * v = std::get_if<boolean>(&m_value)) {
        return **v;
      }
      return !is<nil>();
    }
    [[nodiscard]] int to_int() const noexcept {
      if (const auto * v = std::get_if<number>(&m_value)) {
        return **v;
      }
      return 0;
    }
    [[nodiscard]] std::string to_symbol() const noexcept {
      if (const auto * v = std::get_if<symbol>(&m_value)) {
        return **v;
      }
      return "";
    }
  };
}
namespace mal::types::details {
  struct lambda_ret_t {
    std::shared_ptr<env> e;
    type t;
  };

  [[nodiscard]] static lambda_t convert(std::function<type(lambda_args_t)> fn) noexcept {
    return [fn = std::move(fn)](lambda_args_t args) noexcept {
      return lambda_ret_t { {}, fn(args) };
    };
  }
}
namespace mal {
  using type = types::type;
}
