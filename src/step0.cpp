#include "mal/prompt.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <string>

struct context {
  llvm::LLVMContext ctx {};
  std::unique_ptr<llvm::Module> m = std::make_unique<llvm::Module>("mal", ctx);

  llvm::FunctionType * fn_tp { llvm::FunctionType::get(llvm::Type::getInt8PtrTy(ctx), false) };
  llvm::Function * fn { llvm::Function::Create(fn_tp, llvm::Function::InternalLinkage, "", *m) };
  llvm::IRBuilder<> builder { ctx };
};
using context_ptr = std::unique_ptr<context>;

auto read(const std::string & s) {
  auto c = std::make_unique<context>();

  auto * bb = llvm::BasicBlock::Create(c->ctx, "entry", c->fn);
  c->builder.SetInsertPoint(bb);

  c->builder.CreateRet(c->builder.CreateGlobalStringPtr(s));

  return std::move(c);
}
auto eval(context_ptr c) {
  return c;
}
std::string print(context_ptr c) {
  if (llvm::verifyModule(*c->m, &llvm::errs())) return "Failed to generate valid code";

  std::unique_ptr<llvm::ExecutionEngine> ee { llvm::EngineBuilder { std::move(c->m) }.create() };
  if (ee == nullptr) return "Failure creating JIT engine";

  return static_cast<const char *>(ee->runFunction(c->fn, {}).PointerVal); // NOLINT
}

auto rep(const std::string & s) {
  return print(eval(read(s)));
}

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  mal::prompt::loop(rep);

  llvm::llvm_shutdown();
}
