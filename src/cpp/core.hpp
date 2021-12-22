#pragma once

#include "printer.hpp"
#include "types.hpp"

#include <iostream>
#include <numeric>
#include <sstream>

namespace mal::core::details {
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

  static void setup_step2_funcs(auto & e) {
    e.set("+", types::lambda { details::int_bifunc(std::plus<>()) });
    e.set("-", types::lambda { details::int_bifunc(std::minus<>()) });
    e.set("*", types::lambda { details::int_bifunc(std::multiplies<>()) });
    e.set("/", types::lambda { details::int_bifunc(std::divides<>()) });
  }

  static void setup_step4_funcs(auto & e) {
    setup_step2_funcs(e);
    e.set("pr-str", types::lambda { pr_str });
    e.set("str", types::lambda { str });
    e.set("prn", types::lambda { details::cout_pr_str(true) });
    e.set("println", types::lambda { details::cout_pr_str(false) });
  }
}
