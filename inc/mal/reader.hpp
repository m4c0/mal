#pragma once

#include "mal/parser.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace mal {
  template<typename Visitor>
  class reader {
    using rtype = std::invoke_result_t<Visitor, int>;

    Visitor m_vis;

  public:
    constexpr explicit reader(Visitor v) : m_vis(v) {
    }

    parser::result<rtype> operator()(parser::input_t in) const noexcept;

    constexpr auto read_atom() const noexcept {
      return (parser::boolean & m_vis) | (parser::keyword & m_vis) | (parser::nill & m_vis) | (parser::number & m_vis)
           | (parser::str & m_vis) | (parser::symbol & m_vis);
    }

    constexpr auto read_list() const noexcept {
      return parser::list(*this) & m_vis;
    }
    constexpr auto read_hashmap() const noexcept {
      return parser::hashmap(*this) & m_vis;
    }
    constexpr auto read_vector() const noexcept {
      return parser::vector(*this) & m_vis;
    }
    constexpr auto read_container() const noexcept {
      return read_list() | read_hashmap() | read_vector();
    }

    constexpr auto read_quote(auto p, auto && token) const noexcept {
      const auto t = parser::token<void> { token };
      const auto init = parser::producer([v = m_vis, t] {
        mal::list<rtype> r {};
        return r + v(t);
      });
      return p & (init + *this) & m_vis;
    }
    constexpr auto read_rmacro() const noexcept {
      return read_quote(parser::squote, "quote") | read_quote(parser::backtick, "quasiquote")
           | read_quote(parser::tilde_at, "splice-unquote") | read_quote(parser::tilde, "unquote")
           | read_quote(parser::at, "deref");
    }

    constexpr auto read_meta() const noexcept {
      const auto t = parser::token<void> { "with-meta" };
      const auto builder = [v = m_vis, t](auto map, auto vec) {
        mal::list<rtype> r {};
        return r + v(t) + std::move(vec) + std::move(map);
      };
      return parser::circ & parser::combine(read_hashmap(), parser::trash & read_vector(), builder) & m_vis;
    }

    constexpr auto read_form() const noexcept {
      return parser::trash & (read_atom() | read_container() | read_rmacro() | read_meta());
    }
  };

  template<typename Visitor>
  inline parser::result<typename reader<Visitor>::rtype> reader<Visitor>::operator()(
      parser::input_t in) const noexcept {
    const auto p = reader { m_vis }.read_form();
    return p(in);
  }

  template<typename Visitor>
  static auto read_str(const std::string & s, Visitor vis) {
    using rtype = std::invoke_result_t<Visitor, int>;

    auto t = reader { vis }({ s.data(), s.length() });
    return t % [vis = std::forward<Visitor>(vis)](auto v) noexcept -> std::optional<rtype> {
      if constexpr (std::is_same_v<decltype(v), parser::input_t>) {
        return {};
      } else {
        return v;
      }
    };
  }
}
