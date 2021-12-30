#include "eval.hpp"
#include "eval.impl.hpp"
#include "eval.macro.hpp"

using namespace mal;

type mal::EVAL(const type & in, std::shared_ptr<env> e) {
  type var = in;
  while (e) {
    evals::macro::macroexpand(&var, e);
    e = impl::eval::visit(&var, e);
  }
  return var;
}
