#pragma once

#include "parser.common.hpp"

namespace mal::parser::wrap {
  template<typename T>
  class int_op_wrap {
    type m_value;

  public:
    constexpr int_op_wrap() noexcept = default;
    constexpr explicit int_op_wrap(type v) noexcept : m_value { v } {
    }

    T operator+(type v) const noexcept;

    [[nodiscard]] static constexpr auto unwrap() noexcept {
      return [](T v) noexcept {
        return v.m_value;
      };
    }
  };
  struct sum : int_op_wrap<sum> {
    using int_op_wrap::int_op_wrap;
  };
  struct sub : int_op_wrap<sub> {
    using int_op_wrap::int_op_wrap;
  };
  struct mult : int_op_wrap<mult> {
    using int_op_wrap::int_op_wrap;
  };
  struct div : int_op_wrap<div> {
    using int_op_wrap::int_op_wrap;
  };

  type constant(int v) noexcept;
}
