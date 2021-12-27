#pragma once

#include "core.hpp"

#include <iterator>

namespace mal::core {
  static type cons(std::span<const type> args) noexcept {
    if (args.size() != 2) return types::error { "cons needs a value and a list" };
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

  static void setup_step7_funcs(auto rep, auto & e) {
    setup_step6_funcs(rep, e);

    e->set("cons", types::lambda { cons });
    e->set("concat", types::lambda { concat });
  }
}
