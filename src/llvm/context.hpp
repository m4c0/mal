#pragma once

#include "llvm/IR/LLVMContext.h"

namespace mal {
  class context {
    llvm::LLVMContext m_ctx {};

  public:
    [[nodiscard]] llvm::LLVMContext & ctx() noexcept {
      return m_ctx;
    }

    static context * instance() noexcept {
      static context i {};
      return &i;
    }
  };
}
