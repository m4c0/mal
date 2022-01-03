#include "core.hpp"
#include "gc_object.hpp"

#include <iostream>

extern "C" const char * mal_intr_readline(const char * prompt) noexcept {
  std::cout << prompt;

  std::string line;
  if (!std::getline(std::cin, line)) return "";

  auto * buf = new (mal::gc {}) char[line.length() + 1]; // NOLINT
  std::string_view { buf, line.length() } = line;
  return buf;
}
extern "C" void mal_intr_prstr_int(int i) noexcept {
  std::cout << i << "\n";
}
extern "C" void mal_intr_prstr_list(llvm::Value ** l) noexcept {
  std::cout << "(";
  std::cout << ")\n";
}
