#pragma once

#include "types.hpp"

namespace mal::evals {
  using iteration = types::details::lambda_ret_t;
  using form_input = const types::list &;

  [[nodiscard]] static iteration err_i(const std::string & msg) noexcept {
    return iteration { {}, err(msg) };
  }
}
