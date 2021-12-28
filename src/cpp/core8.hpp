#pragma once

#include "core7.hpp"

namespace mal::core {
  static type nth(std::span<const type> args) noexcept {
    if (!args[1].is<types::number>()) return types::error { "second arg of nth must be a number" };

    auto list = args[0].to_iterable();
    auto index = args[1].to_int();
    if (index >= list.size()) return types::error { "trying to nth outside list/vector bounds" };
    if (index < 0) return types::error { "trying to nth outside list/vector bounds" };
    return list[index];
  }
  static type first(std::span<const type> args) noexcept {
    auto list = args[0].to_iterable();
    if (list.empty()) return types::nil {};
    return list[0];
  }
  static type rest(std::span<const type> args) noexcept {
    auto list = args[0].to_iterable();
    if (list.empty()) return types::list {};
    return types::list { list.subspan(1) };
  }

  static void setup_step8_funcs(auto rep, auto & e) noexcept {
    setup_step7_funcs(rep, e);

    e->set("nth", types::lambda { nth });
    e->set("first", types::lambda { first });
    e->set("rest", types::lambda { rest });
  }
}
