#pragma once

#include "env.hpp"

namespace mal {
  std::string rep(const std::string & in, senv e);
  void run(senv e);
  void run(int argc, char ** argv, senv e);
  void run_with_banner(int argc, char ** argv, senv e);
}
