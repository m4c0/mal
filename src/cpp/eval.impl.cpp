#include "eval.hpp"
#include "eval.impl.hpp"

using namespace mal;

type mal::EVAL(const type & in, std::shared_ptr<env> e) {
  type var = in;
  while (e) {
    auto iter = var.visit(impl::eval { e });
    e = iter.e;
    var = iter.t;
  }
  return var;
}
