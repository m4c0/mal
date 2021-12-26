#pragma once

#include "log.hpp"
#include "printer.hpp"
#include "reader.hpp"
#include "types.hpp"

#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>

namespace mal::core::details {
  template<typename Op>
  [[nodiscard]] static auto bool_bifunc(Op && op) {
    return [op](std::span<const type> args) noexcept -> type {
      if (args.size() != 2) return types::error { "Operation requires two operands" };

      int a = args[0].to_int();
      int b = args[1].to_int();
      return types::boolean { op(a, b) };
    };
  }
  template<typename Op>
  [[nodiscard]] static auto int_bifunc(Op && op) {
    return [op](std::span<const type> args) noexcept -> type {
      if (args.size() < 2) return types::error { "Operation requires at least two operands" };

      int i = args[0].to_int();
      int res = std::accumulate(args.begin() + 1, args.end(), i, [op](int a, const auto & b) noexcept {
        return op(a, b.to_int());
      });
      return types::number { res };
    };
  }

  static void pr_str(std::ostream & os, std::span<const type> args, const std::string & sep, bool readably) noexcept {
    bool first = true;
    for (const auto & a : args) {
      if (!first) os << sep;
      os << mal::pr_str(a, readably);
      first = false;
    }
  }

  static auto cout_pr_str(bool readably) noexcept {
    return [readably](std::span<const type> args) noexcept -> type {
      details::pr_str(std::cout, args, " ", readably);
      std::cout << "\n";
      return types::nil {};
    };
  }
}
namespace mal::core {
  static type pr_str(std::span<const type> args) noexcept {
    std::ostringstream os;
    details::pr_str(os, args, " ", true);
    return types::string { str { os.str() } };
  }
  static type str(std::span<const type> args) noexcept {
    std::ostringstream os;
    details::pr_str(os, args, "", false);
    return types::string { mal::str { os.str() } };
  }

  static type is_empty(std::span<const type> args) noexcept {
    if (args.empty()) return types::boolean { false };
    if (args[0].is<types::list>()) {
      const auto & list = (*args[0].as<types::list>()).peek();
      return types::boolean { list.empty() };
    }
    if (args[0].is<types::vector>()) {
      const auto & list = (*args[0].as<types::vector>()).peek();
      return types::boolean { list.empty() };
    }
    return types::boolean { false };
  }
  static type is_list(std::span<const type> args) noexcept {
    return types::boolean { !args.empty() && args[0].is<types::list>() };
  }

  static type count(std::span<const type> args) noexcept {
    if (args.empty()) return types::number { 0 };
    if (args[0].is<types::list>()) {
      return types::number { static_cast<int>((*args[0].as<types::list>()).peek().size()) };
    }
    if (args[0].is<types::vector>()) {
      return types::number { static_cast<int>((*args[0].as<types::vector>()).peek().size()) };
    }
    return types::number { 0 };
  }

  static type list(std::span<const type> args) noexcept {
    mal::list<type> res {};
    for (const auto & a : args) {
      res = res + a;
    }

    return types::list { std::move(res) };
  }

  static type equal(std::span<const type> args) noexcept {
    if (args.size() != 2) return types::error { "Can only compare two values" };
    return types::boolean { args[0] == args[1] };
  }

  static auto eval(const std::shared_ptr<env> & e) noexcept {
    return [e](std::span<const type> args, auto /*call_env*/) noexcept -> types::details::lambda_ret_t {
      if (args.size() != 1) return { {}, types::error { "Can only eval a single value" } };
      log::debug() << "eval " << e.get() << "\n";
      return { e, args[0] };
    };
  }
  static type read_string(std::span<const type> args) noexcept {
    if (args.size() != 1) return types::error { "Can only read a single string" };
    return read_str({ args[0].to_string() });
  }
  static type slurp(std::span<const type> args) noexcept {
    if (args.size() != 1) return types::error { "Can only slurp a single file" };

    std::ifstream in { args[0].to_string() };
    if (!in) return types::error { "Failure to slurp file" };

    std::ostringstream os {};
    os << in.rdbuf();
    return types::string { mal::str { os.str() } };
  }

  static type atom(std::span<const type> args) noexcept {
    if (args.size() != 1) return types::error { "Can only atomise a single value" };
    return types::atom { args[0] };
  }
  static type is_atom(std::span<const type> args) noexcept {
    if (args.size() != 1) return types::error { "Can only test a single value" };
    return types::boolean { args[0].is<types::atom>() };
  }
  static type deref(std::span<const type> args) noexcept {
    if (args.size() != 1) return types::error { "Can only deref a single value" };
    return *(args[0].as<types::atom>());
  }
  static type reset(std::span<const type> args) noexcept {
    if (args.size() != 2) return types::error { "reset! needs an atom and a value" };
    args[0].as<types::atom>().reset(args[1]);
    return args[1];
  }

  static void setup_step2_funcs(auto & e) {
    e->set("+", types::lambda { details::int_bifunc(std::plus<>()) });
    e->set("-", types::lambda { details::int_bifunc(std::minus<>()) });
    e->set("*", types::lambda { details::int_bifunc(std::multiplies<>()) });
    e->set("/", types::lambda { details::int_bifunc(std::divides<>()) });
  }

  static void setup_step4_funcs(auto rep, auto & e) {
    setup_step2_funcs(e);
    e->set("list", types::lambda { list });
    e->set("list?", types::lambda { is_list });
    e->set("empty?", types::lambda { is_empty });
    e->set("count", types::lambda { count });

    e->set("=", types::lambda { equal });

    e->set("<", types::lambda { details::bool_bifunc(std::less<>()) });
    e->set("<=", types::lambda { details::bool_bifunc(std::less_equal<>()) });
    e->set(">", types::lambda { details::bool_bifunc(std::greater<>()) });
    e->set(">=", types::lambda { details::bool_bifunc(std::greater_equal<>()) });

    e->set("pr-str", types::lambda { pr_str });
    e->set("str", types::lambda { str });
    e->set("prn", types::lambda { details::cout_pr_str(true) });
    e->set("println", types::lambda { details::cout_pr_str(false) });

    rep("(def! not (fn* (a) (if a false true)))", e);
  }

  static void setup_step6_funcs(auto rep, auto & e) {
    setup_step4_funcs(rep, e);

    e->set("read-string", types::lambda { read_string });
    e->set("eval", types::lambda { 0, eval(e) });
    e->set("slurp", types::lambda { slurp });

    e->set("atom", types::lambda { atom });
    e->set("atom?", types::lambda { is_atom });
    e->set("deref", types::lambda { deref });
    e->set("reset!", types::lambda { reset });

    rep(R"--((def! load-file (fn* (f) (eval (read-string (str "(do " (slurp f) "\nnil)"))))))--", e);
  }
}
