#include "eval.hpp"
#include "eval.trycatch.hpp"

using namespace mal::evals;

[[nodiscard]] iteration trycatch::try_(form_input list, senv e) noexcept {
  if (list.empty()) return err("try* requires at least one argument");

  auto res = EVAL(list.at(1), e);
  if (!res.is_error()) return { {}, res };
  if (list.size() == 2) return { {}, res }; // try* without catch*

  auto ne = std::make_shared<env>(e);
  ne->set("*EXCEPTION*", res);
  return { ne, list.at(2) };
}
[[nodiscard]] iteration trycatch::catch_(form_input list, senv e) noexcept {
  if (list.size() < 2) return err("catch* requires at least one symbol and one value");

  if (!list.at(1).is<types::symbol>()) return err("catch* requires a symbol");

  auto ne = std::make_shared<env>(e);
  ne->set(list.at(1).to_symbol(), e->get("*EXCEPTION*"));
  return { ne, list.at(2) };
}
