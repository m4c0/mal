#pragma once
#include "mal/list.hpp"

#include <m4c0/parser/combiners.hpp>
#include <m4c0/parser/str.hpp>
#include <m4c0/parser/tokeniser.hpp>
#include <vector>

namespace mal::parser {
  using namespace m4c0::parser;

  static constexpr const auto dquote = match('"');
  static constexpr const auto qchar = skip(match('\\') & any_char());
  static constexpr const auto nonqchar = skip(match_none_of("\\\""));

  static constexpr const auto lparen = skip(match('('));
  static constexpr const auto rparen = skip(match(')'));

  static constexpr const auto space = many(skip(match_any_of(" \t\n\r,")));
  static constexpr const auto tilde_at = tokenise<void>(match("~@"));
  static constexpr const auto special = tokenise<void>(match_any_of("[]{}'`~^@"));
  static constexpr const auto str = tokenise<void>(dquote & many(qchar | nonqchar) & dquote);
  static constexpr const auto comment = tokenise<void>(match(';') & many(skip(match_none_of("\n\r"))));
  static constexpr const auto other = tokenise<void>(many(skip(match_none_of(" \t\r\n[]{}('\"`,;)]"))));

  static constexpr const auto atom = tilde_at | special | str | comment | other;

  template<typename Fn>
  static constexpr auto list(Fn && form) {
    return lparen + many(form) + rparen;
  }
}
