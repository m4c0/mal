#pragma once

#include "env.hpp"
#include "types.hpp"

#include <memory>

namespace mal {
  static type EVAL(const type & in, const std::shared_ptr<mal::env> & e);
}
