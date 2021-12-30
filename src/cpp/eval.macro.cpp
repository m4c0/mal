#include "eval.hpp"
#include "eval.macro.hpp"
#include "log.hpp"

using namespace mal::evals;

[[nodiscard]] iteration macro::defmacro(form_input in, senv e) noexcept {
  const auto & list = *in;
  if (list.size() != 3) return err_i("defmacro! must have a symbol and a value");

  auto key = list[1].to_symbol();
  if (key.empty()) return err_i("defmacro! can only be called for symbols");

  log::debug() << "defmacro! " << e.get() << " " << key << "\n";

  auto lambda = EVAL(list[2], e);
  if (lambda.is_error()) return { {}, lambda };
  if (!lambda.is<types::lambda>()) return err_i("defmacro! only accepts fn* as value");

  type value = types::macro { lambda.as<types::lambda>() };
  e->set(key, value);
  return { {}, value };
}

[[nodiscard]] static std::optional<mal::type> is_macro_call(const mal::type & ast, senv e) noexcept {
  if (!ast.is<mal::types::list>()) return {};

  const auto & list = ast.as<mal::types::list>();
  if (list.empty()) return {};

  const auto sym = list.at(0).to_symbol();
  auto res = e->get(sym);
  if (res.is<mal::types::macro>()) return res;
  return {};
}
void macro::macroexpand(type * ast, senv e) noexcept {
  while (auto macro = is_macro_call(*ast, e)) {
    auto args = ast->to_iterable().subspan(1);
    auto res = (**macro->as<types::macro>())(args, e);
    *ast = EVAL(res.t, res.e);
  }
}
