#include "mal/context.hpp"
#include "mal/env.hpp"
#include "mal/prompt.hpp"
#include "mal/reader.hpp"

#include <cstdio>
#include <iterator>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

extern "C" const char * mal_i2s(int i) {
  static char buf[1024];               // NOLINT
  snprintf(buf, sizeof(buf), "%d", i); // NOLINT
  return buf;                          // NOLINT
}

class eval_ast {
  using result_t = llvm::Value *;

  mal::context * m_c;
  mal::env * m_e;

public:
  constexpr eval_ast(mal::context * c, mal::env * e) : m_c(c), m_e(e) {
  }

  result_t operator()(const mal::list<result_t> & items) const noexcept {
    auto size = items.end() - items.begin();

    // TODO: new "empty list" or something
    if (size == 0) return nullptr;

    auto * v = *items.begin();
    if (v == nullptr) return nullptr; // TODO: "symbol not found"
    if (!llvm::isa<llvm::Function>(v)) return nullptr;

    auto * fn = dyn_cast<llvm::Function>(v);
    if (size - 1 != fn->arg_size()) return nullptr; // TODO: "mismatched arg size"

    std::vector<result_t> args {};
    std::copy(items.begin() + 1, items.end(), std::back_inserter(args));
    return m_c->builder.CreateCall(fn, args);
  }
  result_t operator()(const mal::hashmap<result_t> & items) const noexcept {
    return nullptr;
  }
  result_t operator()(const mal::vector<result_t> & items) const noexcept {
    return nullptr;
  }
  // keyword
  result_t operator()(mal::parser::token<mal::parser::kw> t) const noexcept {
    // TODO: introduce a custom type
    return m_c->builder.CreateGlobalStringPtr({ t.value.begin(), t.value.length() });
  }
  // symbol
  result_t operator()(mal::parser::token<void> t) const noexcept {
    return m_e->lookup({ t.value.begin(), t.value.length() });
  }
  // string (unencoded)
  result_t operator()(mal::str t) const noexcept {
    return m_c->builder.CreateGlobalStringPtr(*t);
  }
  // boolean (duh)
  result_t operator()(bool b) const noexcept {
    return llvm::ConstantInt::getBool(m_c->ctx, b);
  }
  // nil (duh)
  result_t operator()(const mal::parser::nil & /*v*/) const noexcept {
    return llvm::ConstantTokenNone::get(m_c->ctx);
  }
  // numbers
  result_t operator()(int i) const noexcept {
    auto * i32 = llvm::IntegerType::getInt32Ty(m_c->ctx);
    return llvm::ConstantInt::getSigned(i32, i);
  }
};

std::string rep(const std::string & s) {
  auto c = std::make_unique<mal::context>();

  llvm::PassManagerBuilder pmb;
  pmb.OptLevel = 3;
  pmb.SizeLevel = 0;
  pmb.Inliner = llvm::createFunctionInliningPass(3, 0, false);
  pmb.LoopVectorize = true;
  pmb.SLPVectorize = true;

  llvm::legacy::FunctionPassManager fpm { c->m.get() };
  pmb.populateFunctionPassManager(fpm);
  fpm.doInitialization();

  llvm::legacy::PassManager mpm;
  pmb.populateModulePassManager(mpm);

  auto * bb = llvm::BasicBlock::Create(c->ctx, "entry");
  c->builder.SetInsertPoint(bb);

  mal::env e { *c };

  auto res = mal::read_str(s, eval_ast { c.get(), &e });
  if (!res) {
    llvm::errs() << res.takeError() << "\n";
    return "ERROR";
  }

  auto * res_tp = res.get()->getType();

  llvm::FunctionType * i2s_tp { llvm::FunctionType::get(c->i8p, { res_tp }, false) };
  llvm::Function * i2s_fn { llvm::Function::Create(i2s_tp, llvm::Function::ExternalLinkage, "mal_i2s", *c->m) };
  c->builder.CreateRet(c->builder.CreateCall(i2s_fn, { res.get() }));

  llvm::FunctionType * fn_tp { llvm::FunctionType::get(c->i8p, false) };
  llvm::Function * fn { llvm::Function::Create(fn_tp, llvm::Function::ExternalLinkage, "mal_main", *c->m) };
  c->builder.GetInsertBlock()->insertInto(fn);

  if (llvm::verifyModule(*c->m, &llvm::errs())) return "Failed to generate valid code";
  fpm.run(*fn);
  mpm.run(*c->m);

  c->m->print(llvm::errs(), nullptr);

  auto * ee = llvm::EngineBuilder { std::move(c->m) }.create();
  if (ee == nullptr) return "Failure creating JIT engine";

  return static_cast<const char *>(ee->runFunction(fn, {}).PointerVal); // NOLINT
}

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  mal::prompt::loop(rep);

  llvm::llvm_shutdown();
}
