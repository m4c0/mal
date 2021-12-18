#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace mal {
  struct context {
    llvm::LLVMContext ctx {};
    std::unique_ptr<llvm::Module> m = std::make_unique<llvm::Module>("mal", ctx);

    llvm::Type * i8p { llvm::Type::getInt8PtrTy(ctx) };

    llvm::IRBuilder<> builder { ctx };
  };
}
