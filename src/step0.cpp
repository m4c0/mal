#include "mal/prompt.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <string>

auto read(const std::string & s) {
  return s;
}
auto eval(const std::string & s) {
  return s;
}
auto print(const std::string & s) {
  return s;
}

auto rep(const std::string & s) {
  return print(eval(read(s)));
}

int main() {
  llvm::LLVMContext ctx;
  llvm::Module m { "mal", ctx };
  llvm::IRBuilder<> builder { ctx };

  mal::prompt::loop(rep);
}
