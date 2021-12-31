#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

namespace mal::log {
#ifdef NDEBUG
  struct dummy {
    template<typename Tp>
    constexpr dummy operator<<(Tp && /*n*/) const noexcept {
      return {};
    }
  };

  static auto debug() noexcept {
    return dummy {};
  }
#else
  static auto & debug() noexcept {
    return std::cerr;
  }
#endif
}
