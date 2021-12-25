#pragma once

#include <iostream>

namespace mal::log {
  static auto & debug() noexcept {
    return std::cerr;
  }
}
