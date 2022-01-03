#pragma once

#include "parser.common.hpp"

namespace mal::parser::wrap {
  template<typename T>
  class int_op_wrap {
    type m_value;

  public:
    T operator+(type v) const noexcept;

    [[nodiscard]] static constexpr auto unwrap() noexcept {
      return [](T v) noexcept {
        return v.m_value;
      };
    }
  };
  struct sum : int_op_wrap<sum> {};
  struct sub : int_op_wrap<sub> {};
  struct mult : int_op_wrap<mult> {};
  struct div : int_op_wrap<div> {};

  type constant(int v) noexcept;
}
