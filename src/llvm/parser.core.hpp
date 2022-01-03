#pragma once

#include "parser.wrap.hpp"

namespace mal::parser {
  template<is_parser Form>
  class core {
    template<typename Agg>
    static constexpr auto int_agg() noexcept {
      return many(space & Form {}, Agg {}) & Agg::unwrap();
    }

  public:
    static constexpr const auto sum = match('+') & int_agg<wrap::sum>();
    static constexpr const auto subtract = match('-') & int_agg<wrap::sub>();
    static constexpr const auto multiply = match('*') & int_agg<wrap::mult>();
    static constexpr const auto divide = match('/') & int_agg<wrap::div>();

    static constexpr const auto any = sum | subtract | multiply | divide;
  };
}
