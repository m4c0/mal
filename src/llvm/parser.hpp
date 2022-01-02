#include "m4c0/parser/numeric.hpp"
#include "m4c0/parser/str.hpp"

namespace mal::parser {
  using namespace m4c0::parser;
}
namespace mal::parser::recursion {
  struct form {
    constexpr result<nil> operator()(input_t in) const noexcept;
  };
}
namespace mal::parser {
  // Shamelessly avoiding <functional> due to its compilation weight
  struct multiplies {
    [[nodiscard]] constexpr auto operator()(auto a, auto b) const noexcept {
      return a * b;
    }
  };

  static constexpr auto lparen = match('(');
  static constexpr auto rparen = match(')');

  static constexpr auto plus = match('+');
  static constexpr auto minus = match('-');
  static constexpr auto mult = match('*');
  static constexpr auto div = match('/');

  static constexpr auto blanks = many(skip(match_any_of(" \r\n\t,")));

  static constexpr auto sign = minus & -1 | 1;
  static constexpr auto number = combine(sign, match_u32(), multiplies());

  static constexpr auto form = blanks & recursion::form();
}
namespace mal::parser::core {
  static constexpr auto sum = plus & many(form);
  static constexpr auto subtract = minus & many(form);
  static constexpr auto multiply = mult & many(form);
  static constexpr auto divide = div & many(form);

  static constexpr auto any = sum | subtract | multiply | divide;
}
namespace mal::parser {
  static constexpr auto list = lparen & blanks & core::any & blanks & rparen;

  static constexpr auto non_form = skip(list) | skip(number);

  constexpr result<nil> recursion::form::operator()(input_t in) const noexcept {
    return non_form(in);
  }
  static constexpr auto program = many(skip(form));
}
