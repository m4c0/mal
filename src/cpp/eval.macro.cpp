#include "eval.hpp"
#include "eval.macro.hpp"
#include "log.hpp"

using namespace mal::evals;

[[nodiscard]] iteration macro::defmacro(form_input in, senv e) noexcept {
  const auto & list = *in;
  if (list.size() != 3) return err("defmacro! must have a symbol and a value");

  auto key = list[1].to_symbol();
  if (key.empty()) return err("defmacro! can only be called for symbols");

  log::debug() << "defmacro! " << e.get() << " " << key << "\n";

  auto lambda = EVAL(list[2], e);
  if (lambda.is_error()) return { {}, lambda };
  if (!lambda.is<types::lambda>()) return err("defmacro! only accepts fn* as value");

  type value = types::macro { lambda.as<types::lambda>() };
  e->set(key, value);
  return { {}, value };
}
