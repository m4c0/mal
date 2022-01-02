#pragma once

#include "list.hpp"
#include "str.hpp"

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
  static constexpr const auto space = at_least_one(skip(match_any_of(" \t\n\r,")));
  static constexpr const auto trash = many(comment | space);

  static constexpr const auto other = at_least_one(skip(match_none_of(" \t\r\n[]{}('\"`,;)]:~@^")));

  static constexpr const auto keyword = tokenise<kw>(match(':') & other);

  static constexpr const auto sign = match('-') & -1 | 1;
  static constexpr const auto number = combine(sign, match_u32(), [](int a, int b) {
    return a * b;
  });

  static constexpr const auto symbol = tokenise<void>(other);

  static constexpr const auto no_qmark = look_ahead(match_none_of("?")) | eof();

  static constexpr const auto b_true = match("true") + no_qmark & true;
  static constexpr const auto b_false = match("false") + no_qmark & false;
  static constexpr const auto boolean = b_true | b_false;

  static constexpr const auto nill = skip(match("nil")) + no_qmark;

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
