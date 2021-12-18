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

  llvm::IRBuilder<> builder { ctx };
};
using context_ptr = std::unique_ptr<context>;

auto read(mal::context * c, const std::string & s) {
  return c->builder.CreateGlobalStringPtr(s);
}
auto eval(auto c) {
  return c;
}
auto print(auto c) {
  return c;
}

std::string rep(std::unique_ptr<mal::context> c, const std::string & s) {
  auto * bb = llvm::BasicBlock::Create(c->ctx, "entry");
  c->builder.SetInsertPoint(bb);

  llvm::FunctionType * fn_tp { llvm::FunctionType::get(llvm::Type::getInt8PtrTy(c->ctx), false) };
  llvm::Function * fn { llvm::Function::Create(fn_tp, llvm::Function::InternalLinkage, "", *c->m) };
  c->builder.GetInsertBlock()->insertInto(fn);

  c->builder.CreateRet(print(eval(read(c.get(), s))));

  if (llvm::verifyModule(*c->m, &llvm::errs())) return "Failed to generate valid code";

  std::unique_ptr<llvm::ExecutionEngine> ee { llvm::EngineBuilder { std::move(c->m) }.create() };
  if (ee == nullptr) return "Failure creating JIT engine";

  return static_cast<const char *>(ee->runFunction(fn, {}).PointerVal); // NOLINT
}

int main() {
  mal::prompt::loop(rep);
}
