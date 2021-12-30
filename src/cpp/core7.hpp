#pragma once

#include "core.hpp"
#include "eval.quasiquote.hpp"

#include <iterator>

namespace mal::core {
  static type cons(std::span<const type> args) noexcept {
    if (args.size() != 2) return err("cons needs a value and a list");
    return types::list { args[0], args[1].to_iterable() };
  }

  static type concat(std::span<const type> args) noexcept {
    std::vector<type> res {};
    for (const auto & arg : args) {
      auto it = arg.to_iterable();
      std::copy(it.begin(), it.end(), std::back_inserter(res));
    }
    return types::list { std::move(res) };
  }

  static type vec(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("vec needs an interable");
    if (args[0].is<types::vector>()) return args[0];
    if (!args[0].is<types::list>()) return err("vec can't work on this");
    return types::vector { args[0].to_iterable() };
  }

  static void setup_step7_funcs(auto rep, auto & e) {
    setup_step6_funcs(rep, e);

    e->set("quote", types::special { evals::quote });
    e->set("quasiquoteexpand", types::special { evals::quasiquoteexpand_form });
    e->set("quasiquote", types::special { evals::quasiquote_form });

    e->set("cons", types::lambda { cons });
    e->set("concat", types::lambda { concat });
    e->set("vec", types::lambda { vec });
  }
}
