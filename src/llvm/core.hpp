#pragma once

namespace llvm {
  class Value;
}
namespace mal {
  class string;
}

extern "C" const char * mal_intr_readline(const char * prompt) noexcept;
extern "C" void mal_intr_prstr_int(int i) noexcept;
extern "C" void mal_intr_prstr_list(llvm::Value ** l) noexcept;
