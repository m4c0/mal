#pragma once

#include "env.hpp"
#include "types.hpp"

#include <memory>

namespace mal {
  [[nodiscard]] type EVAL(const type & in, std::shared_ptr<mal::env> e);
}
