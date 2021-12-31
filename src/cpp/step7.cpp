#include "core7.hpp"
#include "env.hpp"

int main(int argc, char ** argv) {
  auto e = mal::env::make();
  mal::core::setup_step7_funcs(e);
  mal::run(argc, argv, e);
}
