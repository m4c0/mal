#pragma once

#include "eval.common.hpp"
#include "types.hpp"

namespace mal::evals {
  struct macro {
    [[nodiscard]] static iteration defmacro(form_input in, senv e) noexcept;
    [[nodiscard]] static type macroexpand(type ast, senv e) noexcept;
  };
}
