#include "context.hpp"
#include "parser.hpp"

#include <iostream>
#include <stdexcept>

using namespace mal;

struct finish {
  void operator()(parser::type v) {
    auto * prstr = mod()->getFunction("mal_intr_prstr_int");
    builder().CreateCall(prstr, { v });
    builder().CreateRetVoid();

    context::instance()->end();
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
  using namespace mal::parser;
  constexpr const form parser {};

  std::string line;
  while (readline(line)) {
    context::instance()->begin();

    try {
      parser(input_t { line.data(), line.size() }) % finish {};
    } catch (const std::exception & e) {
      std::cerr << "error: " << e.what() << "\n";
    }
  }
}
