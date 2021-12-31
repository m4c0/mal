#pragma once

#include "env.hpp"
#include "types.hpp"

#include <memory>

namespace mal {
  [[nodiscard]] type EVAL(const type & in, senv e);
}
