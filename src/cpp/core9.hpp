#pragma once

#include "core8.hpp"
#include "printer.hpp"
#include "types.hpp"

namespace mal::core {
  static type throw_(std::span<const type> args) noexcept {
    return types::error { args[0] };
  }
  static types::details::lambda_ret_t apply(std::span<const type> args, const std::shared_ptr<env> & e) noexcept {
    if (args.size() < 2) return { {}, err("apply requires at least two arguments") };
    if (!args[0].is<types::lambda>()) return { {}, err("apply require a function as first argument") };
    if (!args.back().is_iterable()) return { {}, err("apply require a list/vector as last argument") };

    return { e, types::list { args[0], args.subspan(1, args.size() - 2), args.back().to_iterable() } };
  }
  static type map(std::span<const type> args) noexcept {
    return {};
  }

  static void setup_step9_funcs(auto rep, auto & e) noexcept {
    setup_step8_funcs(rep, e);

    e->set("throw", types::lambda { throw_ });
    e->set("apply", types::lambda { 0, apply });
    e->set("map", types::lambda { map });
  }
}
