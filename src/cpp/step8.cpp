#include "core8.hpp"
#include "env.hpp"

int main(int argc, char ** argv) {
  auto e = mal::env::make();
  mal::core::setup_step8_funcs(e);
  mal::run(argc, argv, e);
}
