#include "coreA.hpp"
#include "env.hpp"
#include "rep.hpp"

#include <memory>

int main(int argc, char ** argv) {
  auto e = mal::env::make();
  mal::core::setup_stepA_funcs(e);
  mal::run_with_banner(argc, argv, e);
}
