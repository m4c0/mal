#pragma once

namespace mal {
  class string;
}

extern "C" void mal_intr_readline(const mal::string * prompt, mal::string * line) noexcept;
extern "C" void mal_intr_prstr_int(int i) noexcept;
