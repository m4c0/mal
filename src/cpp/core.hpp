#pragma once

#include "types.hpp"

#include <numeric>

namespace mal::core {
  template<typename Op>
  [[nodiscard]] static auto int_bifunc(Op && op) {
    return [op](std::span<type> args) noexcept -> type {
      if (args.size() < 2) return types::error { "Operation requires at least two operands" };

      int i = to_int(args[0]);
      int res = std::accumulate(args.begin() + 1, args.end(), i, [op](int a, const auto & b) noexcept {
        return op(a, to_int(b));
      });
      return types::number { res };
    };
  }
  template<typename Op>
  [[nodiscard]] static type int_bifunc(std::span<type> args, Op && op) {
    if (args.size() < 2) return types::error { "Operation requires at least two operands" };

    int i = to_int(args[0]);
    int res = std::accumulate(args.begin() + 1, args.end(), i, [op](int a, const auto & b) noexcept {
      return op(a, to_int(b));
    });
    return types::number { res };
  }

  static void setup_step2_funcs(auto & e) {
    e.emplace("+", types::lambda { int_bifunc(std::plus<>()) });
    e.emplace("-", types::lambda { int_bifunc(std::minus<>()) });
    e.emplace("*", types::lambda { int_bifunc(std::multiplies<>()) });
    e.emplace("/", types::lambda { int_bifunc(std::divides<>()) });
  }
}
