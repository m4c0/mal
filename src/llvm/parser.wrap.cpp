#include "parser.wrap.hpp"

namespace mal::parser::wrap {
  type constant(int v) noexcept {
    return nullptr;
  }

  template<>
  sum int_op_wrap<sum>::operator+(type v) const noexcept {
    return sum {};
  }
  template<>
  sub int_op_wrap<sub>::operator+(type v) const noexcept {
    return sub {};
  }
  template<>
  mult int_op_wrap<mult>::operator+(type v) const noexcept {
    return mult {};
  }
  template<>
  div int_op_wrap<div>::operator+(type v) const noexcept {
    return div {};
  }
}
