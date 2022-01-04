#pragma once

#include "parser.common.hpp"
#include "parser.core.hpp"

namespace mal::parser {
  template<char C>
  static constexpr const auto l = match(C) & opt_space;
  template<char C>
  static constexpr const auto r = opt_space & skip(match(C));

  static constexpr const auto lparen = l<'('>;
  static constexpr const auto rparen = r<')'>;
  static constexpr const auto lsq = l<'['>;
  static constexpr const auto rsq = r<']'>;

  struct form {
    constexpr result<type> operator()(input_t in) const;
  };

  static constexpr const auto eval_list = core<form>::any + rparen;
  static constexpr const auto empty_list = rparen & wrap::empty_list;
  static constexpr const auto list = lparen & (eval_list | empty_list);
  static constexpr const auto vector = lsq & rsq & wrap::empty_vector;

  static constexpr const auto number = match_s32() & wrap::constant;

  static constexpr const auto non_form = list | vector | number | "Invalid form";

  constexpr result<type> form::operator()(input_t in) const {
    return non_form(in);
  }
}
