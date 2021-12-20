#pragma once

#include "mal/context.hpp"

#include <iostream>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <string>
#include <type_traits>

namespace mal::prompt::impl {
  static auto & read_user_input(std::string & line) {
    std::cout << "user> ";
    return std::getline(std::cin, line);
  }
  template<typename Fn>
  static auto run(Fn && rep, const std::string & line, int opt_level) {
    auto c = std::make_unique<context>();

    llvm::FunctionType * fn_tp { llvm::FunctionType::get(c->i8p, false) };
    llvm::Function * fn { llvm::Function::Create(fn_tp, llvm::Function::ExternalLinkage, "mal_main", *c->m) };
    c->builder.SetInsertPoint(llvm::BasicBlock::Create(c->ctx, "entry", fn));

    auto exp_res = rep(c.get(), line);
    if (!exp_res) {
      llvm::errs() << exp_res.takeError() << "\n";
      return "ERROR";
    }

    c->builder.CreateRet(exp_res.get());

    if (llvm::verifyModule(*c->m, &llvm::errs())) return "Failed to generate valid code";

    llvm::PassManagerBuilder pmb;
    pmb.OptLevel = opt_level;
    pmb.SizeLevel = 0;
    pmb.Inliner = llvm::createFunctionInliningPass(3, 0, false);
    pmb.LoopVectorize = true;
    pmb.SLPVectorize = true;

    llvm::legacy::FunctionPassManager fpm { c->m.get() };
    pmb.populateFunctionPassManager(fpm);
    fpm.doInitialization();
    fpm.run(*fn);

    llvm::legacy::PassManager mpm;
    pmb.populateModulePassManager(mpm);
    mpm.run(*c->m);

    c->m->print(llvm::errs(), nullptr);

    auto * ee = llvm::EngineBuilder { std::move(c->m) }.create();
    if (ee == nullptr) return "Failure creating JIT engine";

    return static_cast<const char *>(ee->runFunction(fn, {}).PointerVal); // NOLINT
  }
}
namespace mal::prompt {
  template<typename Fn>
  requires std::is_invocable_r_v<llvm::Expected<llvm::Value *>, Fn, context *, std::string> static void loop(
      Fn && rep,
      int opt_level = 3) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    while (std::cin) {
      for (std::string line; impl::read_user_input(line);) {
        std::cout << impl::run(rep, line, opt_level) << "\n";
      }
    }

    llvm::llvm_shutdown();
  }
}
