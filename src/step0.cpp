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
  return c->builder.CreateGlobalStringPtr(s, "", 0, c->m.get());
}
auto eval(auto c) {
  return c;
}
auto print(auto c) {
  return c;
}

llvm::Expected<llvm::Value *> rep(mal::context * c, const std::string & s) {
  return print(eval(read(c, s)));
}

int main() {
  mal::prompt::loop(rep);
}
