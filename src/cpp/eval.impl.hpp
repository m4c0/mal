#pragma once

#include "env.hpp"
#include "eval.common.hpp"
#include "eval.hpp"
#include "eval_ast.hpp"
#include "types.hpp"

#include <utility>

namespace mal::impl {
  class eval {
    static std::shared_ptr<env> eval_list(type * v, evals::senv e) noexcept {
      *v = v->visit(eval_ast { e });
      if (v->is_error()) return {};

      const auto & list = *v->as<types::list>();
      if (!list.at(0).is<types::lambda>()) {
        *v = err("Can't run that");
        return {};
      }

      auto oper = list.at(0).as<types::lambda>();
      auto args = std::span(list).subspan(1);
      auto res = (*oper)(args, e);
      *v = res.t;
      return res.e;
    }

  public:
    [[nodiscard]] static std::shared_ptr<env> visit(type * v, const std::shared_ptr<env> & e) noexcept {
      if (!v->is<types::list>()) {
        *v = v->visit(eval_ast { e });
        return {};
      }
      auto args = v->to_iterable();
      if (args.empty()) return {};

      auto spc = e->get(args[0].to_symbol());

      evals::iteration it;
      if (spc.is<types::special>()) {
        it = (*spc.as<types::special>())(v->as<types::list>(), e);
        *v = it.t;
        return it.e;
      }

      return eval_list(v, e);
    }
  };
}
