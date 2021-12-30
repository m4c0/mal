#pragma once

#include "eval.common.hpp"
#include "types.hpp"

namespace mal::evals {
  struct macro {
    [[nodiscard]] static iteration defmacro(form_input in, senv e) noexcept;
    static void macroexpand(type * ast, senv e) noexcept;

    static iteration macroexpand_form(form_input in, senv e) noexcept {
      type ast = in.at(1);
      macroexpand(&ast, e);
      return { {}, ast };
    }
  };
}
