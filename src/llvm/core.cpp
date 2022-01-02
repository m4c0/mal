#include "core.hpp"
#include "string.hpp"

#include <iostream>

extern "C" void mal_intr_readline(const mal::string * prompt, mal::string * line) noexcept {
  std::cout << **prompt;

  std::string buf;
  *line = std::getline(std::cin, buf) ? mal::string { buf } : mal::string {};
}
