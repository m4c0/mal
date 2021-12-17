#pragma once

#include "mal/parser.hpp"
#include "mal/types.hpp"

#include <llvm/Support/Error.h>
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
    constexpr auto read_form() const noexcept {
      return parser::trash & (read_atom() | read_list() | read_hashmap() | read_vector());
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
    return t % [vis = std::forward<Visitor>(vis)](auto v) noexcept -> llvm::Expected<rtype> {
      if constexpr (std::is_same_v<decltype(v), parser::input_t>) {
        // return "EOF" for all parse errors until we get a real parse error.
        // this needs a parser rewrite to make "decisive" errors, like unbalanced parenthesis and quotes
        return llvm::createStringError(llvm::inconvertibleErrorCode(), "EOF");
      } else {
        return v;
      }
    };
  }
}
