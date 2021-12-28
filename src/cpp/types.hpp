#pragma once

#include "types.holders.hpp"

#include <functional>
#include <iterator>
#include <span>
#include <type_traits>
#include <utility>
#include <variant>

namespace mal {
  class env;
}
namespace mal::types::details {
  struct lambda_ret_t;
  using lambda_args_t = std::span<const type>;
  using lambda_t = std::function<lambda_ret_t(lambda_args_t, std::shared_ptr<env>)>;
  [[nodiscard]] static lambda_t convert(std::function<type(lambda_args_t)> fn) noexcept;
}
namespace mal::types {
  struct atom : details::heavy_holder<type> {
    using heavy_holder::heavy_holder;
  };
  struct error : details::heavy_holder<type> {
    using heavy_holder::heavy_holder;
  };

  struct list : details::vector_holder<list> {
    using vector_holder::vector_holder;
  };
  struct vector : details::vector_holder<vector> {
    using vector_holder::vector_holder;
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
  using macro = details::holder<lambda>;
  using nil = nullptr_t;
  using number = details::holder<int>;
  using string = details::heavy_holder<std::string>;
  using symbol = details::token_holder<void>;

  [[nodiscard]] static bool operator==(const lambda & /*a*/, const lambda & /*b*/) noexcept {
    return false;
  }

  template<typename... Tps>
  struct type_variant_vararg_helper {
    using variant_t = std::variant<Tps...>;

    template<typename Vis>
    constexpr static const bool can_be_visited_by = (std::is_invocable_v<Vis, Tps> && ...);
  };
  using type_variant_helper = type_variant_vararg_helper<
      nil,
      atom,
      boolean,
      error,
      hashmap,
      macro,
      number,
      keyword,
      lambda,
      list,
      string,
      symbol,
      vector>;
  using type_variant_t = type_variant_helper::variant_t;

  template<typename Tp>
  concept subtype = requires(type_variant_t v) {
    { std::get<std::decay_t<Tp>>(v) };
  };
  template<typename Vis>
  concept type_visitor = type_variant_helper::can_be_visited_by<Vis>;

  class type {
    type_variant_t m_value {};

  public:
    constexpr type() = default;

    template<subtype Tp>
    type(Tp v) : m_value { std::move(v) } { // NOLINT - we want the simplicity of implicit
    }

    [[nodiscard]] bool operator==(const type & o) const noexcept {
      if (is_iterable() && o.is_iterable()) {
        auto me = to_iterable();
        auto them = o.to_iterable();
        return std::equal(me.begin(), me.end(), them.begin(), them.end());
      }
      if (!is_iterable() && !o.is_iterable()) {
        return m_value == o.m_value;
      }
      return false;
    }

    template<typename Tp>
    [[nodiscard]] Tp & as() noexcept {
      return std::get<Tp>(m_value);
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
    [[nodiscard]] constexpr bool is_iterable() const noexcept {
      return std::holds_alternative<list>(m_value) || std::holds_alternative<vector>(m_value);
    }

    template<type_visitor Visitor>
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
    [[nodiscard]] std::string to_string() const noexcept {
      if (const auto * v = std::get_if<string>(&m_value)) {
        return **v;
      }
      return "";
    }
    [[nodiscard]] std::span<const type> to_iterable() const noexcept {
      if (const auto * v = std::get_if<list>(&m_value)) {
        return **v;
      }
      if (const auto * v = std::get_if<vector>(&m_value)) {
        return **v;
      }
      return {};
    }
  };
}
namespace mal::types::details {
  struct lambda_ret_t {
    std::shared_ptr<env> e;
    type t;
  };

  [[nodiscard]] static lambda_t convert(std::function<type(lambda_args_t)> fn) noexcept {
    return [fn = std::move(fn)](lambda_args_t args, const std::shared_ptr<env> & /**/) noexcept {
      return lambda_ret_t { {}, fn(args) };
    };
  }
}
namespace mal {
  using type = types::type;

  [[nodiscard]] static type err(const std::string & msg) noexcept {
    return types::error { types::string { msg } };
  }
}
