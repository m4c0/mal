#include "context.hpp"
#include "core.hpp"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace mal;

static const struct llvm_init { // NOLINT
  llvm_init() noexcept {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
  }
  ~llvm_init() noexcept {
    llvm::llvm_shutdown();
  }
} i;

void context::begin() noexcept {
  m_module = std::make_unique<llvm::Module>("mal", m_ctx);

  auto * fn_tp = llvm::FunctionType::get(llvm::Type::getVoidTy(m_ctx), false);
  m_fn = llvm::Function::Create(fn_tp, llvm::Function::ExternalLinkage, "mal", *m_module);

  m_builder.SetInsertPoint(llvm::BasicBlock::Create(m_ctx, "entry", m_fn));

  fn_tp = llvm_helper::function_type(mal_intr_prstr_int);
  m_module->getOrInsertFunction("mal_intr_prstr_int", llvm_helper::function_type(mal_intr_prstr_int));
}
void context::end() {
  if (llvm::verifyModule(*m_module, &llvm::errs(), nullptr)) {
    throw std::runtime_error("Issues found while compiling");
  }

  llvm::PassManagerBuilder pmb;
  pmb.OptLevel = 3;
  pmb.SizeLevel = 0;
  pmb.Inliner = llvm::createFunctionInliningPass(3, 0, false);
  pmb.LoopVectorize = true;
  pmb.SLPVectorize = true;

  llvm::legacy::FunctionPassManager fpm { m_module.get() };
  pmb.populateFunctionPassManager(fpm);
  fpm.doInitialization();
  fpm.run(*m_fn);

  llvm::legacy::PassManager mpm;
  pmb.populateModulePassManager(mpm);
  mpm.run(*m_module);

  m_module->print(llvm::errs(), nullptr);

  auto * ee = llvm::EngineBuilder { std::move(m_module) }.create();
  if (ee == nullptr) throw std::runtime_error("Failure creating JIT engine");
  ee->runFunction(m_fn, {});
}
