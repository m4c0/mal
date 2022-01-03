#include "context.hpp"
#include "parser.wrap.hpp"

#include "llvm/IR/Constants.h"

namespace mal::parser::wrap {
  type constant(int v) noexcept {
    auto & ctx = context::instance()->ctx();
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx), v);
  }

  template<>
  sum int_op_wrap<sum>::operator+(type v) const noexcept {
    return sum { v };
  }
  template<>
  sub int_op_wrap<sub>::operator+(type v) const noexcept {
    return sub { v };
  }
  template<>
  mult int_op_wrap<mult>::operator+(type v) const noexcept {
    return mult { v };
  }
  template<>
  div int_op_wrap<div>::operator+(type v) const noexcept {
    return div { v };
  }
}
