#pragma once

#include "eval.common.hpp"
#include "types.hpp"

namespace mal::evals {
  struct trycatch {
    [[nodiscard]] static iteration try_(form_input list, senv e) noexcept;
    [[nodiscard]] static iteration catch_(form_input list, senv e) noexcept;
  };
}
