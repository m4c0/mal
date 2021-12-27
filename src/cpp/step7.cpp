#include "core.hpp"
#include "env.hpp"
#include "eval.impl.hpp"
#include "printer.hpp"
#include "reader.hpp"

#include <iostream>
#include <sstream>
#include <string>

static auto READ(auto in) {
  return mal::read_str(in);
}
static auto PRINT(auto in) {
  return mal::pr_str(in);
}

static auto rep(const std::string & in, const std::shared_ptr<mal::env> & e) {
  return PRINT(EVAL(READ(in), e));
}

static auto readline(std::string & line) {
  std::cout << "user> ";

  return static_cast<bool>(std::getline(std::cin, line));
}
int main(int argc, char ** argv) {
  auto e = std::make_shared<mal::env>();
  mal::core::setup_step7_funcs(rep, e);

  std::span<char *> argn { argv, static_cast<size_t>(argc) };

  mal::list<mal::type> arg_list;
  for (int i = 2; i < argn.size(); i++) {
    arg_list = arg_list + mal::type { mal::types::string { mal::str { argn[i] } } };
  }
  e->set("*ARGV*", mal::type { mal::types::list { std::move(arg_list) } });

  if (argn.size() > 1) {
    std::ostringstream os;
    os << "(load-file \"" << argn[1] << "\")";
    std::cout << rep(os.str(), e) << "\n";
    return 0;
  }

  std::string line;
  while (readline(line)) {
    std::cout << rep(line, e) << "\n";
  }
}
