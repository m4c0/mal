#pragma once

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

namespace mal {
  class list;
  class map;
  class vector;

  class context {
    llvm::LLVMContext m_ctx {};
    std::unique_ptr<llvm::Module> m_module {};
    llvm::IRBuilder<> m_builder { m_ctx };
    llvm::Function * m_fn {};

    llvm::Type * m_list_tp { llvm::StructType::create(m_ctx, "mal_type_list") };
    llvm::Type * m_map_tp { llvm::StructType::create(m_ctx, "mal_type_map") };
    llvm::Type * m_vector_tp { llvm::StructType::create(m_ctx, "mal_type_vector") };

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

    [[nodiscard]] auto * list_type() noexcept {
      return m_list_tp;
    }
    [[nodiscard]] auto * map_type() noexcept {
      return m_map_tp;
    }
    [[nodiscard]] auto * vector_type() noexcept {
      return m_vector_tp;
    }

    [[nodiscard]] static context * instance() noexcept {
      static context i {};
      return &i;
    }
  };

  namespace llvm_helper {
    static constexpr auto get_struct_by_name(const char * name) noexcept {
      return [name](auto & ctx) noexcept {
        return llvm::StructType::getTypeByName(ctx, name);
      };
    }

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
    struct type_fns<mal::list *> {
      static constexpr const auto getter = get_struct_by_name("mal_type_list");
    };
    template<>
    struct type_fns<mal::map *> {
      static constexpr const auto getter = get_struct_by_name("mal_type_map");
    };
    template<>
    struct type_fns<mal::vector *> {
      static constexpr const auto getter = get_struct_by_name("mal_type_vector");
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
