#pragma once

#include "mal/list.hpp"
#include "mal/str.hpp"

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

  class kw {};

  static constexpr const auto squote = match('\'');
  static constexpr const auto backtick = match('`');
  static constexpr const auto tilde = match('~');
  static constexpr const auto tilde_at = match("~@");
  static constexpr const auto at = match('@');

  static constexpr const auto circ = match('^');

  static constexpr const auto dquote = skip(match('"'));
  static constexpr const auto qcr = match("\\r") & '\r';
  static constexpr const auto qlf = match("\\n") & '\n';
  static constexpr const auto qq = match("\\\\") & '\\';
  static constexpr const auto qd = match("\\\"") & '"';

  static constexpr const auto qchar = qcr | qlf | qq | qd;
  static constexpr const auto nonqchar = match_none_of("\\\"");
  static constexpr const auto str = dquote + (producer_of<mal::str>() << (qchar | nonqchar)) + dquote;

  static constexpr const auto lparen = skip(match('('));
  static constexpr const auto rparen = skip(match(')'));
  static constexpr const auto lsq = skip(match('['));
  static constexpr const auto rsq = skip(match(']'));
  static constexpr const auto lbr = skip(match('{'));
  static constexpr const auto rbr = skip(match('}'));

  static constexpr const auto comment = match(';') & many(skip(match_none_of("\n\r")));
  static constexpr const auto space = many(skip(match_any_of(" \t\n\r,")));
  static constexpr const auto trash = comment | space;

  static constexpr const auto other = at_least_one(skip(match_none_of(" \t\r\n[]{}('\"`,;)]:~@^")));

  static constexpr const auto keyword = tokenise<kw>(match(':') & other);

  static constexpr const auto sign = match('-') & -1 | 1;
  static constexpr const auto number = combine(sign, match_u32(), [](int a, int b) {
    return a * b;
  });

  static constexpr const auto symbol = tokenise<void>(other);

  static constexpr const auto b_true = match("true") & true;
  static constexpr const auto b_false = match("false") & false;
  static constexpr const auto boolean = b_true | b_false;

  static constexpr const auto nill = skip(match("nil"));

  template<typename Form>
  static constexpr auto list(Form && form) {
    constexpr const auto init = parser::producer_of<mal::list<type_of_t<Form>>>();
    return lparen + (init << form) + trash + rparen;
  }
  template<typename Form>
  static constexpr auto hashmap(Form && form) {
    constexpr const auto init = parser::producer_of<mal::hashmap<type_of_t<Form>>>();
    constexpr const auto key = tokenise<void>(str) | tokenise<void>(keyword);
    constexpr const auto entry = [](auto && k, auto && v) {
      std::string kstr { k.value.begin(), k.value.length() };
      return mal::hashmap_entry<type_of_t<Form>> { kstr, std::forward<decltype(v)>(v) };
    };
    return lbr + (init << (trash & combine(key, form, entry))) + trash + rbr;
  }
  template<typename Form>
  static constexpr auto vector(Form && form) {
    constexpr const auto init = parser::producer_of<mal::vector<type_of_t<Form>>>();
    return lsq + (init << form) + trash + rsq;
  }
}
