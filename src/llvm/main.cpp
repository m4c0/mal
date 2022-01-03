#include "context.hpp"
#include "parser.hpp"

#include <iostream>
#include <stdexcept>

using namespace mal;

static const char * pr_str_for_type(parser::type v) {
  if (v->getType()->isIntegerTy()) return "mal_intr_prstr_int";
  if (v->getType() == llvm::IntegerType::getInt32PtrTy(context::instance()->ctx())) return "mal_intr_prstr_list";
  throw std::runtime_error("unknown type");
}

struct finish {
  void operator()(parser::type v) {
    const char * pr_str = pr_str_for_type(v);
    builder().CreateCall(mod()->getFunction(pr_str), { v });
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
