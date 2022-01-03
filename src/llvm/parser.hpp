#pragma once

#include "m4c0/parser/numeric.hpp"
#include "m4c0/parser/str.hpp"

namespace mal::parser {
  using namespace m4c0::parser;
  using type = int;
}
namespace mal::parser::recursion {
  struct form {
    constexpr result<type> operator()(input_t in) const;
  };
}
namespace mal::parser {
  static constexpr const auto blanks = many(skip(match_any_of(" \r\n\t,")));

  static constexpr const auto lparen = match('(') & blanks;
  static constexpr const auto rparen = blanks & skip(match(')'));

  static constexpr const auto plus = match('+');
  static constexpr const auto minus = match('-');
  static constexpr const auto mult = match('*');
  static constexpr const auto div = match('/');

  static constexpr const auto number = match_s32();

  static constexpr const auto form = blanks & recursion::form();
}
namespace mal::parser::core {
  type to_int(type value);

  static constexpr const auto int_form = form & to_int;

  static constexpr const auto sum = plus & many(int_form);
  static constexpr const auto subtract = minus & int_form & many(int_form);
  static constexpr const auto multiply = mult & many(int_form);
  static constexpr const auto divide = div & int_form & many(int_form);

  static constexpr const auto any = sum | subtract | multiply | divide;
}
namespace mal::parser {
  static constexpr const auto empty_list = lparen & rparen & 99;
  static constexpr const auto list = lparen & core::any & rparen & 88;

  static constexpr const auto non_form = empty_list | list | number;

  constexpr result<type> recursion::form::operator()(input_t in) const {
    return non_form(in);
  }
}
