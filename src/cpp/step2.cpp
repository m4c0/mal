#include "core.hpp"
#include "env.hpp"
#include "rep.hpp"

int main() {
  auto e = mal::env::make();
  mal::core::setup_step2_funcs(e);
  mal::run(e);
}
