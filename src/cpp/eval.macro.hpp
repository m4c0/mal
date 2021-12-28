#pragma once

#include "eval.common.hpp"
#include "types.hpp"

namespace mal::evals {
  struct macro {
    [[nodiscard]] static iteration defmacro(form_input in, senv e) noexcept;
  };
}
