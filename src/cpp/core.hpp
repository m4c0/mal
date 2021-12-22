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

  static void setup_step2_funcs(auto & e) {
    e.set("+", types::lambda { details::int_bifunc(std::plus<>()) });
    e.set("-", types::lambda { details::int_bifunc(std::minus<>()) });
    e.set("*", types::lambda { details::int_bifunc(std::multiplies<>()) });
    e.set("/", types::lambda { details::int_bifunc(std::divides<>()) });
  }

  static void setup_step4_funcs(auto & e) {
    setup_step2_funcs(e);
    e.set("list", types::lambda { list });
    e.set("list?", types::lambda { is_list });
    e.set("empty?", types::lambda { is_empty });
    e.set("count", types::lambda { count });

    e.set("pr-str", types::lambda { pr_str });
    e.set("str", types::lambda { str });
    e.set("prn", types::lambda { details::cout_pr_str(true) });
    e.set("println", types::lambda { details::cout_pr_str(false) });
  }
}
