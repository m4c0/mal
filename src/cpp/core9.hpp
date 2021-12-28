#pragma once

#include "core8.hpp"
#include "types.hpp"

namespace mal::core {
  static type throw_(std::span<const type> args) noexcept {
    return {};
  }
  static type apply(std::span<const type> args) noexcept {
    return {};
  }
  static type map(std::span<const type> args) noexcept {
    return {};
  }

  static void setup_step9_funcs(auto rep, auto & e) noexcept {
    setup_step7_funcs(rep, e);

    e->set("throw", types::lambda { throw_ });
    e->set("apply", types::lambda { apply });
    e->set("map", types::lambda { map });
  }
}
