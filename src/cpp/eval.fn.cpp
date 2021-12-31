#include "env.hpp"
#include "eval.common.hpp"
#include "eval.fn.hpp"
#include "eval.hpp"
#include "log.hpp"
#include "types.hpp"

#include <iterator>

namespace mal::evals::details {
  static auto fn_lambda(senv oe, const std::vector<type> & params, const type & body) {
    return [oe, params, body](std::span<const type> args, auto /**/) -> types::details::lambda_ret_t {
      auto e = std::make_shared<env>(oe);

      for (int i = 0; i < params.size(); i++) {
        auto p = params[i].to_symbol();
        if (p.empty()) return err_i("fn* without a symbol parameter");
        if (p == "&") {
          if (params.size() != i + 2) return err_i("fn* missing bind after &");

          p = params[i + 1].to_symbol();
          if (p.empty()) return err_i("fn* without a symbol parameter after &");

          e->set(p, types::list { args.subspan(i) });
          break;
        }
        if (args.size() <= i) return err_i("fn* argument list differs in size from actual call");
        e->set(p, args[i]);
      }
      return { e, body };
    };
  }
}
namespace mal::evals {
  [[nodiscard]] iteration fn(const types::list & in, senv oe) noexcept {
    const auto & list = *in;
    if (list.size() != 3) return err_i("fn* must have parameters and body");

    auto p_span = list[1].to_iterable();
    std::vector<type> params;
    params.reserve(p_span.size());
    std::copy(p_span.begin(), p_span.end(), std::back_inserter(params));

    log::debug() << "fn* " << oe.get() << "\n";
    return { {}, types::lambda { 0, details::fn_lambda(oe, params, list[2]) } };
  }
}
