#pragma once

#include "core.hpp"

namespace mal::core {
  static type cons(std::span<const type> args) noexcept {
    if (args.size() != 2) return types::error { "cons needs a value and a list" };

    auto res = mal::list<type> {} + args[0] + args[1].to_iterable();
    return types::list { std::move(res) };
  }

  static type concat(std::span<const type> args) noexcept {
    mal::list<type> res {};
    for (const auto & arg : args) {
      res = res + arg.to_iterable();
    }
    return types::list { std::move(res) };
  }

  static void setup_step7_funcs(auto rep, auto & e) {
    setup_step6_funcs(rep, e);

    e->set("cons", types::lambda { cons });
    e->set("concat", types::lambda { concat });
  }
}
