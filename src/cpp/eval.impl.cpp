#include "eval.hpp"
#include "eval.impl.hpp"
#include "eval.macro.hpp"

using namespace mal;

type mal::EVAL(const type & in, senv oe) {
  auto e = oe;
  type var = in;
  while (e) {
    evals::macro::macroexpand(&var, e);
    e = impl::eval::visit(&var, e);
  }
  return var;
}
