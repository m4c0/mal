#include "core9.hpp"
#include "env.hpp"
#include "rep.hpp"

#include <memory>

int main(int argc, char ** argv) {
  auto e = std::make_shared<mal::env>();
  mal::core::setup_step9_funcs(mal::rep, e);
  mal::run(argc, argv, e);
}
