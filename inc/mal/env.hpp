#pragma once

#include "mal/context.hpp"

#include <string_view>
#include <unordered_map>

namespace mal {
  class env {
    using key_t = std::string_view;
    using value_t = llvm::Function *;
    std::unordered_map<key_t, value_t> m_fns {};

    static auto * create_fn(context & c, auto name, auto fn) {
      auto * i32 = llvm::IntegerType::getInt32Ty(c.ctx);
      auto * fn_t = llvm::FunctionType::get(i32, { i32, i32 }, false);

      auto * res = llvm::Function::Create(fn_t, llvm::Function::InternalLinkage, name, *c.m);
      llvm::IRBuilder<> b { c.ctx };
      b.SetInsertPoint(llvm::BasicBlock::Create(c.ctx, "entry", res));
      b.CreateRet(fn(b, res->getArg(0), res->getArg(1)));
      return res;
    }

  public:
    explicit env(context & c) {
      m_fns["+"] = create_fn(c, "+", [](auto & bld, auto a, auto b) {
        return bld.CreateAdd(a, b);
      });
      m_fns["-"] = create_fn(c, "-", [](auto & bld, auto a, auto b) {
        return bld.CreateSub(a, b);
      });
      m_fns["*"] = create_fn(c, "*", [](auto & bld, auto a, auto b) {
        return bld.CreateMul(a, b);
      });
      m_fns["/"] = create_fn(c, "/", [](auto & bld, auto a, auto b) {
        return bld.CreateSDiv(a, b);
      });
    }

    [[nodiscard]] value_t lookup(key_t key) const noexcept {
      auto it = m_fns.find(key);
      if (it == m_fns.end()) return nullptr;
      return it->second;
    }
  };
}
