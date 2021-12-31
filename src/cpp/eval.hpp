#pragma once

#include "env.hpp"
#include "types.hpp"

namespace mal {
  [[nodiscard]] type EVAL(const type & in, senv e);
}
