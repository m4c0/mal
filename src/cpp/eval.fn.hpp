#pragma once

#include "env.hpp"
#include "types.hpp"

namespace mal::evals {
  template<typename Eval>
  [[nodiscard]] static type fn(std::shared_ptr<env> oe, const types::list & in) noexcept {
    const auto & list = (*in).peek();
    if (list.size() != 3) return types::error { "fn* must have parameters and body" };

    const std::vector<type> * params {};
    if (list[1].is<types::list>()) params = &(*list[1].as<types::list>()).peek();
    if (list[1].is<types::vector>()) params = &(*list[1].as<types::vector>()).peek();
    if (params == nullptr) return types::error { "fn* parameters must be a list or vector" };

    const auto l = [oe, params = *params, body = list[2]](std::span<const type> args) noexcept -> type {
      auto it = std::find_if(args.begin(), args.end(), [](auto t) {
        return t.is_error();
      });
      if (it != args.end()) {
        return *it;
      }

      auto e = std::make_shared<env>(oe);

      for (int i = 0; i < params.size(); i++) {
        auto p = params[i].to_symbol();
        if (p.empty()) return types::error { "fn* without a symbol parameter" };
        if (p == "&") {
          if (params.size() != i + 2) return types::error { "fn* missing bind after &" };

          p = params[i + 1].to_symbol();
          if (p.empty()) return types::error { "fn* without a symbol parameter after &" };

          mal::list<type> l;
          for (int j = i; j < args.size(); j++) {
            l = l + args[j];
          }
          e->set(p, types::list { std::move(l) });
          break;
        }
        if (args.size() <= i) return types::error { "fn* argument list differs in size from actual call" };
        e->set(p, args[i]);
      }
      return body.visit(Eval { e });
    };
    return types::lambda { l };
  }
}
