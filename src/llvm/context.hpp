#pragma once

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

namespace mal {
  class context {
    llvm::LLVMContext m_ctx {};
    llvm::Module m_module { "mal", m_ctx };
    llvm::IRBuilder<> m_builder { m_ctx };

    context() {
      auto * fn_tp = llvm::FunctionType::get(llvm::Type::getVoidTy(m_ctx), false);
      auto * fn = llvm::Function::Create(fn_tp, llvm::Function::ExternalLinkage, "", m_module);

      m_builder.SetInsertPoint(llvm::BasicBlock::Create(m_ctx, "entry", fn));
    }

  public:
    [[nodiscard]] auto & builder() noexcept {
      return m_builder;
    }
    [[nodiscard]] auto & ctx() noexcept {
      return m_ctx;
    }

    [[nodiscard]] static context * instance() noexcept {
      static context i {};
      return &i;
    }
  };

  static auto & builder() noexcept {
    return context::instance()->builder();
  }
}
