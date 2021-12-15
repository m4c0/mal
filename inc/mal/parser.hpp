#pragma once

#include "mal/list.hpp"
#include "mal/types.hpp"

#include <functional>
#include <m4c0/parser/combiners.hpp>
#include <m4c0/parser/constants.hpp>
#include <m4c0/parser/numeric.hpp>
#include <m4c0/parser/str.hpp>
#include <m4c0/parser/tokeniser.hpp>
#include <memory>
#include <vector>

namespace mal::parser {
  using namespace m4c0::parser;

  template<typename Tp>
  static constexpr auto as_type() noexcept {
    return [](auto in) noexcept -> u_type {
      return std::make_unique<Tp>(std::move(in));
    };
  }

  static constexpr const auto dquote = match('"');
  static constexpr const auto qchar = skip(match('\\') & any_char());
  static constexpr const auto nonqchar = skip(match_none_of("\\\""));

  static constexpr const auto lparen = skip(match('('));
  static constexpr const auto rparen = skip(match(')'));

  static constexpr const auto space = many(skip(match_any_of(" \t\n\r,")));
  static constexpr const auto tilde_at = skip(match("~@"));
  static constexpr const auto special = skip(match_any_of("[]{}'`~^@"));
  static constexpr const auto str = skip(dquote & many(qchar | nonqchar) & dquote);
  static constexpr const auto comment = match(';') & many(skip(match_none_of("\n\r")));
  static constexpr const auto other = many(skip(match_none_of(" \t\r\n[]{}('\"`,;)]")));

  static constexpr const auto sign = match('-') & -1 | 1;
  static constexpr const auto number = combine(sign, match_u32(), std::multiplies<>()) & as_type<mal::types::number>();

  static constexpr const auto symbol =
      tokenise<void>(tilde_at | special | str | comment | other) & as_type<mal::types::symbol>();

  static constexpr const auto atom = number | symbol;

  template<typename Fn>
  static constexpr auto list(Fn && form) {
    return (lparen + (producer_of<mal::list>() << form) + rparen) & as_type<mal::types::list>();
  }
}
