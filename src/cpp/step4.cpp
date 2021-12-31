#include "core.hpp"
#include "env.hpp"

int main() {
  auto e = mal::env::make();
  mal::core::setup_step4_funcs(e);
  mal::run(e);
}
