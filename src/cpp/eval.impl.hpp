#pragma once

#include "env.hpp"
#include "eval.common.hpp"
#include "eval.hpp"
#include "eval_ast.hpp"
#include "types.hpp"

#include <utility>

namespace mal::impl {
  class eval {
    static evals::iteration eval_list(type * v, evals::senv e) noexcept {
      auto evald = v->visit(eval_ast { e });
      if (evald.is_error()) return { {}, evald };

      const auto & list = *evald.as<types::list>();
      auto it = std::find_if(list.begin(), list.end(), [](auto t) {
        return t.is_error();
      });
      if (it != list.end()) {
        return { {}, *it };
      }

      if (!list.at(0).is<types::lambda>()) return evals::err_i("Can't run that");

      auto oper = list.at(0).as<types::lambda>();
      auto args = std::span(list).subspan(1);
      return (*oper)(args, e);
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
      } else {
        it = eval_list(v, e);
      }

      *v = it.t;
      return it.e;
    }
  };
}
