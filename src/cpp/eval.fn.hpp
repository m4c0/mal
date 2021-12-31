#pragma once

#include "env.hpp"
#include "eval.common.hpp"

namespace mal::evals {
  [[nodiscard]] iteration fn(const types::list & in, senv oe) noexcept;
}
