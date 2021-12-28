#include "eval.hpp"
#include "eval.impl.hpp"
#include "eval.macro.hpp"

using namespace mal;

type mal::EVAL(const type & in, std::shared_ptr<env> e) {
  type var = in;
  while (e) {
    var = evals::macro::macroexpand(var, e);

    auto iter = var.visit(impl::eval { e });
    e = iter.e;
    var = iter.t;
  }
  return var;
}
