#include "parser.hpp"

#include <iostream>
#include <stdexcept>

struct finish {
  void operator()(mal::parser::type v) noexcept {
    std::cerr << v << "\n";
  }
  void operator()(mal::parser::input_t err) noexcept {
    std::cerr << "error: " << std::string_view { err.begin(), err.length() } << "\n";
  }
};

static bool readline(std::string & line) noexcept {
  std::cerr << "user> ";
  return !!std::getline(std::cin, line);
}

int main() {
  constexpr const mal::parser::form parser {};
  std::string line;
  while (readline(line)) {
    using namespace mal::parser;

    try {
      parser(input_t { line.data(), line.size() }) % finish {};
    } catch (const std::exception & e) {
      std::cerr << "error: " << e.what() << "\n";
    }
  }
}
