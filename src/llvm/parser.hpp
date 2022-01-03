#pragma once

#include "parser.common.hpp"
#include "parser.core.hpp"

namespace mal::parser {
  static constexpr const auto lparen = match('(') & opt_space;
  static constexpr const auto rparen = opt_space & skip(match(')'));

  struct form {
    constexpr result<type> operator()(input_t in) const;
  };
}

namespace mal::parser {
  static constexpr const auto eval_list = core<form>::any + rparen;
  static constexpr const auto empty_list = rparen & wrap::empty_list;
  static constexpr const auto list = lparen & (eval_list | empty_list);

  static constexpr const auto number = match_s32() & wrap::constant;

  static constexpr const auto non_form = list | number | "Invalid form";

  constexpr result<type> form::operator()(input_t in) const {
    return non_form(in);
  }
}
