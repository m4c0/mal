#pragma once

#include "core8.hpp"
#include "printer.hpp"
#include "types.hpp"

#include <algorithm>
#include <iterator>

namespace mal::core {
  static type throw_(std::span<const type> args) noexcept {
    return types::error { args[0] };
  }
  static types::details::lambda_ret_t apply(std::span<const type> args, const std::shared_ptr<env> & e) noexcept {
    if (args.size() < 2) return { {}, err("apply requires at least two arguments") };
    if (!args[0].is<types::lambda>()) return { {}, err("apply require a function as first argument") };
    if (!args.back().is_iterable()) return { {}, err("apply require a list/vector as last argument") };

    const auto & fn = *args[0].as<types::lambda>();
    const auto fn_args = types::list { args.subspan(1, args.size() - 2), args.back().to_iterable() };
    return fn(*fn_args, e);
  }
  static types::details::lambda_ret_t map(std::span<const type> args, const std::shared_ptr<env> & e) noexcept {
    if (args.size() != 2) return { {}, err("map requires at two arguments") };
    if (!args[0].is<types::lambda>()) return { {}, err("map require a function as first argument") };
    if (!args[1].is_iterable()) return { {}, err("map require a list/vector as last argument") };

    const auto & fn = *args[0].as<types::lambda>();

    auto values = args[1].to_iterable();

    std::vector<type> res;
    res.reserve(values.size());

    for (const auto & v : values) {
      auto fn_res = fn(std::span { &v, 1 }, e);
      auto new_v = EVAL(fn_res.t, fn_res.e);
      if (new_v.is_error()) return { {}, new_v };
      res.push_back(new_v);
    }
    return { {}, types::list { res } };
  }
  static type is_nil(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::nil>() };
  }
  static type is_true(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].to_boolean(false) };
  }
  static type is_false(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && !args[0].to_boolean(false) };
  }
  static type is_symbol(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::symbol>() };
  }

  static void setup_step9_funcs(auto rep, auto & e) noexcept {
    setup_step8_funcs(rep, e);

    e->set("throw", types::lambda { throw_ });
    e->set("apply", types::lambda { 0, apply });
    e->set("map", types::lambda { 0, map });

    e->set("nil?", types::lambda { is_nil });
    e->set("true?", types::lambda { is_true });
    e->set("false?", types::lambda { is_false });
    e->set("symbol?", types::lambda { is_symbol });
  }
}
