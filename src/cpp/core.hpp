#pragma once

#include "eval.fn.hpp"
#include "eval.hpp"
#include "eval.specials.hpp"
#include "log.hpp"
#include "printer.hpp"
#include "reader.hpp"
#include "rep.hpp"
#include "types.hpp"

#include <numeric>

namespace mal::core::details {
  template<typename Op>
  [[nodiscard]] static auto bool_bifunc(Op && op) {
    return [op](std::span<const type> args) noexcept -> type {
      if (args.size() != 2) return err("Operation requires two operands");

      int a = args[0].to_int();
      int b = args[1].to_int();
      return types::boolean { op(a, b) };
    };
  }
  template<typename Op>
  [[nodiscard]] static auto int_bifunc(Op && op) {
    return [op](std::span<const type> args) noexcept -> type {
      if (args.size() < 2) return err("Operation requires at least two operands");

      int i = args[0].to_int();
      int res = std::accumulate(args.begin() + 1, args.end(), i, [op](int a, const auto & b) noexcept {
        return op(a, b.to_int());
      });
      return types::number { res };
    };
  }

  type prn(std::span<const type> args) noexcept;
  type println(std::span<const type> args) noexcept;
  type pr_str(std::span<const type> args) noexcept;
  type slurp(std::span<const type> args) noexcept;
  type str(std::span<const type> args) noexcept;
}
namespace mal::core {
  static type is_empty(std::span<const type> args) noexcept {
    if (args.empty()) return types::boolean { false };
    return types::boolean { args[0].to_iterable().empty() };
  }
  static type is_list(std::span<const type> args) noexcept {
    return types::boolean { !args.empty() && args[0].is<types::list>() };
  }

  static type count(std::span<const type> args) noexcept {
    if (args.empty()) return types::number { 0 };
    return types::number { static_cast<int>(args[0].to_iterable().size()) };
  }

  static type list(std::span<const type> args) noexcept {
    return types::list { args };
  }

  static type equal(std::span<const type> args) noexcept {
    if (args.size() != 2) return err("Can only compare two values");
    return types::boolean { args[0] == args[1] };
  }

  static auto eval(senv e) noexcept {
    return [e](std::span<const type> args, auto /*call_env*/) noexcept -> types::details::lambda_ret_t {
      if (args.size() != 1) return { {}, err("Can only eval a single value") };
      log::debug() << "eval " << e.get() << "\n";
      return { e, args[0] };
    };
  }
  static type read_string(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("Can only read a single string");
    return read_str({ args[0].to_string() });
  }

  static type atom(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("Can only atomise a single value");
    return types::atom { args[0] };
  }
  static type is_atom(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("Can only test a single value");
    return types::boolean { args[0].is<types::atom>() };
  }
  static type deref(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("Can only deref a single value");
    if (!args[0].is<types::atom>()) return err("deref requires an atom");
    return *(args[0].as<types::atom>());
  }
  static type reset(std::span<const type> args) noexcept {
    if (args.size() != 2) return err("reset! needs an atom and a value");
    if (!args[0].is<types::atom>()) return err("reset! requires an atom");
    return args[0].as<types::atom>().reset(args[1]);
  }
  static types::details::lambda_ret_t swap(std::span<const type> args, senv env) noexcept {
    if (args.size() < 2) return { {}, err("swap! needs at least a atom and a function") };
    if (!args[0].is<types::atom>()) return { {}, err("swap! requires an atom") };
    if (!args[1].is<types::lambda>()) return { {}, err("swap! requires a function") };

    auto atom = args[0].as<types::atom>();
    const auto & fn = args[1].as<types::lambda>();
    types::list fn_args { *atom, args.subspan(2) };
    auto res = (*fn)(std::span(*fn_args), env);
    return { {}, atom.reset(EVAL(res.t, res.e)) };
  }

  static void setup_step2_funcs(auto & e) {
    e->set("+", types::lambda { details::int_bifunc(std::plus<>()) });
    e->set("-", types::lambda { details::int_bifunc(std::minus<>()) });
    e->set("*", types::lambda { details::int_bifunc(std::multiplies<>()) });
    e->set("/", types::lambda { details::int_bifunc(std::divides<>()) });
  }

  static void setup_step3_funcs(auto & e) {
    setup_step2_funcs(e);

    e->set("def!", types::special { evals::def });
    e->set("let*", types::special { evals::let });
  }

  static void setup_step4_funcs(auto & e) {
    setup_step3_funcs(e);

    e->set("do", types::special { evals::do_ });
    e->set("if", types::special { evals::if_ });
    e->set("fn*", types::special { evals::fn });

    e->set("list", types::lambda { list });
    e->set("list?", types::lambda { is_list });
    e->set("empty?", types::lambda { is_empty });
    e->set("count", types::lambda { count });

    e->set("=", types::lambda { equal });

    e->set("<", types::lambda { details::bool_bifunc(std::less<>()) });
    e->set("<=", types::lambda { details::bool_bifunc(std::less_equal<>()) });
    e->set(">", types::lambda { details::bool_bifunc(std::greater<>()) });
    e->set(">=", types::lambda { details::bool_bifunc(std::greater_equal<>()) });

    e->set("pr-str", types::lambda { details::pr_str });
    e->set("str", types::lambda { details::str });
    e->set("prn", types::lambda { details::prn });
    e->set("println", types::lambda { details::println });

    rep("(def! not (fn* (a) (if a false true)))", e);
  }

  static void setup_step6_funcs(auto & e) {
    setup_step4_funcs(e);

    e->set("read-string", types::lambda { read_string });
    e->set("eval", types::lambda { 0, eval(e) });
    e->set("slurp", types::lambda { details::slurp });

    e->set("atom", types::lambda { atom });
    e->set("atom?", types::lambda { is_atom });
    e->set("deref", types::lambda { deref });
    e->set("reset!", types::lambda { reset });
    e->set("swap!", types::lambda { 0, swap });

    rep(R"--((def! load-file (fn* (f) (eval (read-string (str "(do " (slurp f) "\nnil)"))))))--", e);
  }
}
