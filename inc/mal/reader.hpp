#pragma once

#include "mal/parser.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace mal::parser {
  template<typename Visitor>
  class reader {
    using rtype = std::invoke_result_t<Visitor, int>;

    Visitor m_vis;

  public:
    constexpr explicit reader(Visitor v) : m_vis(v) {
    }

    result<rtype> operator()(input_t in) const noexcept;

    constexpr auto read_atom() const noexcept {
      return (boolean & m_vis) | (keyword & m_vis) | (nill & m_vis) | (number & m_vis) | (str & m_vis)
           | (symbol & m_vis);
    }

    constexpr auto read_list() const noexcept {
      return list(*this) & m_vis;
    }
    constexpr auto read_hashmap() const noexcept {
      return hashmap(*this) & m_vis;
    }
    constexpr auto read_vector() const noexcept {
      return vector(*this) & m_vis;
    }
    constexpr auto read_container() const noexcept {
      return read_list() | read_hashmap() | read_vector();
    }

    constexpr auto read_quote(auto p, auto && tk) const noexcept {
      const auto t = token<void> { tk };
      const auto init = producer([v = m_vis, t] {
        mal::list<rtype> r {};
        return r + v(t);
      });
      return p & (init + *this) & m_vis;
    }
    constexpr auto read_rmacro() const noexcept {
      return read_quote(squote, "quote") | read_quote(backtick, "quasiquote") | read_quote(tilde_at, "splice-unquote")
           | read_quote(tilde, "unquote") | read_quote(at, "deref");
    }

    constexpr auto read_meta() const noexcept {
      const auto t = token<void> { "with-meta" };
      const auto builder = [v = m_vis, t](auto map, auto vec) {
        mal::list<rtype> r {};
        return r + v(t) + std::move(vec) + std::move(map);
      };
      return circ & combine(read_hashmap(), trash & read_vector(), builder) & m_vis;
    }

    constexpr auto read_form() const noexcept {
      return trash & (read_atom() | read_container() | read_rmacro() | read_meta());
    }
  };

  template<typename Visitor>
  inline result<typename reader<Visitor>::rtype> reader<Visitor>::operator()(input_t in) const noexcept {
    const auto p = reader { m_vis }.read_form();
    return p(in);
  }

  template<typename Visitor>
  static auto read_str(const std::string & s, Visitor vis) {
    using rtype = std::invoke_result_t<Visitor, int>;

    auto t = reader { vis }({ s.data(), s.length() });
    return t % [vis = std::forward<Visitor>(vis)](auto v) noexcept -> std::optional<rtype> {
      if constexpr (std::is_same_v<decltype(v), input_t>) {
        return {};
      } else {
        return v;
      }
    };
  }
}
