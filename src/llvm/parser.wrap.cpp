#include "context.hpp"
#include "parser.wrap.hpp"

#include "llvm/IR/Constants.h"

namespace mal::parser::wrap {
  type constant(int v) noexcept {
    auto & ctx = context::instance()->ctx();
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(ctx), v);
  }
  type empty_list(nil /**/) noexcept {
    auto & ctx = context::instance()->ctx();
    return llvm::Constant::getNullValue(llvm::Type::getInt32PtrTy(ctx));
  }
  type empty_vector(nil /**/) noexcept {
    auto & ctx = context::instance()->ctx();
    return llvm::Constant::getNullValue(llvm::Type::getInt32PtrTy(ctx));
  }

  template<>
  sum int_op_wrap<sum>::operator+(type v) const noexcept {
    return sum { m_value != nullptr ? builder().CreateAdd(m_value, v) : v };
  }
  template<>
  sub int_op_wrap<sub>::operator+(type v) const noexcept {
    return sub { m_value != nullptr ? builder().CreateSub(m_value, v) : v };
  }
  template<>
  mult int_op_wrap<mult>::operator+(type v) const noexcept {
    return mult { m_value != nullptr ? builder().CreateMul(m_value, v) : v };
  }
  template<>
  div int_op_wrap<div>::operator+(type v) const noexcept {
    return div { m_value != nullptr ? builder().CreateSDiv(m_value, v) : v };
  }
}
