#pragma once

#include "mal/reader.hpp"

namespace mal::type {
  class reader {
  public:
    void * operator()(auto n) const noexcept {
      return nullptr;
    }
  };
};
namespace mal {
  static auto read_str(const std::string & line) {
    auto res = parser::read_str(line, type::reader {});
    return res ? *res : nullptr;
  }
}
