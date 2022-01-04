#pragma once

namespace llvm {
  class Value;
}
namespace mal {
  class list;
  class map;
  class vector;
}

extern "C" const char * mal_intr_readline(const char * prompt) noexcept;
extern "C" void mal_intr_prstr_int(int i) noexcept;
extern "C" void mal_intr_prstr_list(mal::list * l) noexcept;
extern "C" void mal_intr_prstr_map(mal::map * l) noexcept;
extern "C" void mal_intr_prstr_vector(mal::vector * l) noexcept;
