#pragma once

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

namespace mal {
  class context {
    llvm::LLVMContext m_ctx {};
    std::unique_ptr<llvm::Module> m_module {};
    llvm::IRBuilder<> m_builder { m_ctx };
    llvm::Function * m_fn {};

    context() = default;

  public:
    void begin() noexcept;
    void end();

    [[nodiscard]] auto & builder() noexcept {
      return m_builder;
    }
    [[nodiscard]] auto & ctx() noexcept {
      return m_ctx;
    }
    [[nodiscard]] auto * mod() noexcept {
      return m_module.get();
    }

    [[nodiscard]] static context * instance() noexcept {
      static context i {};
      return &i;
    }
  };

  namespace llvm_helper {
    template<typename Ret>
    struct type_fns;
    template<>
    struct type_fns<void> {
      static constexpr const auto getter = llvm::Type::getVoidTy;
    };
    template<>
    struct type_fns<int> {
      static constexpr const auto getter = llvm::Type::getInt32Ty;
    };
    template<>
    struct type_fns<llvm::Value **> {
      static constexpr const auto getter = [](auto & ctx) {
        return llvm::Type::getInt32PtrTy(ctx);
      };
    };

    template<typename Ret, typename... Args>
    [[nodiscard]] static auto * function_type(Ret (*)(Args...), bool vararg = false) noexcept { // NOLINT
      auto & ctx = context::instance()->ctx();
      auto * ret = type_fns<Ret>::getter(ctx);
      return llvm::FunctionType::get(ret, { (type_fns<Args>::getter(ctx), ...) }, vararg);
    }
  }

  static auto * mod() noexcept {
    return context::instance()->mod();
  }
  static auto & builder() noexcept {
    return context::instance()->builder();
  }
}
