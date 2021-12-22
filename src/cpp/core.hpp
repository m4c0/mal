#pragma once

#include "types.hpp"

#include <numeric>

namespace mal::core {
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

  static void setup_step2_funcs(auto & e) {
    e.set("+", types::lambda { int_bifunc(std::plus<>()) });
    e.set("-", types::lambda { int_bifunc(std::minus<>()) });
    e.set("*", types::lambda { int_bifunc(std::multiplies<>()) });
    e.set("/", types::lambda { int_bifunc(std::divides<>()) });
  }
}
