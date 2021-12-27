#pragma once

#include "core.hpp"

namespace mal::core {
  static type cons(std::span<const type> args) noexcept {
    if (args.size() != 2) return types::error { "cons needs a value and a list" };
    if (!args[1].is<types::list>()) return types::error { "cons requires a value" };

    const auto & l = *(args[1].as<types::list>());

    auto res = mal::list<type> {} + args[0] + std::span { l.peek() };
    return types::list { std::move(res) };
  }

  static type concat(std::span<const type> args) noexcept {
    mal::list<type> res {};
    for (const auto & arg : args) {
      if (!arg.is<types::list>()) return types::error { "concat requires a value" };

      res = res + std::span { (*arg.as<types::list>()).peek() };
    }
    return types::list { std::move(res) };
  }

  static void setup_step7_funcs(auto rep, auto & e) {
    setup_step6_funcs(rep, e);

    e->set("cons", types::lambda { cons });
    e->set("concat", types::lambda { concat });
  }
}
