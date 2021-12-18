#pragma once

#include "mal/context.hpp"

#include <iostream>
#include <llvm/Support/TargetSelect.h>
#include <string>
#include <type_traits>

namespace mal::prompt::impl {
  static auto & read_user_input(std::string & line) {
    std::cout << "user> ";
    return std::getline(std::cin, line);
  }
  template<typename Fn>
  static auto run(Fn && rep, const std::string & line) {
    auto c = std::make_unique<context>();
    return rep(std::move(c), line);
  }

}
namespace mal::prompt {
  template<typename Fn>
  requires std::is_invocable_r_v<std::string, Fn, std::unique_ptr<context>, std::string>
  static void loop(Fn && rep) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    while (std::cin) {
      for (std::string line; impl::read_user_input(line);) {
        std::cout << impl::run(rep, line) << "\n";
      }
    }

    llvm::llvm_shutdown();
  }
}
