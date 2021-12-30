#pragma once

#include "core7.hpp"
#include "eval.macro.hpp"

namespace mal::core {
  static type nth(std::span<const type> args) noexcept {
    if (!args[1].is<types::number>()) return err("second arg of nth must be a number");

    auto list = args[0].to_iterable();
    auto index = args[1].to_int();
    if (index >= list.size()) return err("trying to nth outside list/vector bounds");
    if (index < 0) return err("trying to nth outside list/vector bounds");
    return list[index];
  }
  static type first(std::span<const type> args) noexcept {
    auto list = args[0].to_iterable();
    if (list.empty()) return types::nil {};
    return list[0];
  }
  static type rest(std::span<const type> args) noexcept {
    auto list = args[0].to_iterable();
    if (list.empty()) return types::list {};
    return types::list { list.subspan(1) };
  }

  static void setup_step8_funcs(auto rep, auto & e) noexcept {
    setup_step7_funcs(rep, e);

    e->set("defmacro!", types::special { evals::macro::defmacro });
    e->set("macroexpand", types::special { evals::macro::macroexpand_form });

    e->set("nth", types::lambda { nth });
    e->set("first", types::lambda { first });
    e->set("rest", types::lambda { rest });

    constexpr const auto * const cond = R"--(
      (defmacro! cond
        (fn* (& xs)
          (if (> (count xs) 0)
              (list
                'if
                (first xs)
                (if (> (count xs) 1)
                  (nth xs 1)
                  (throw "odd number of forms to cond"))
                (cons 'cond (rest (rest xs))))))))--";
    rep(cond, e);
  }
}
