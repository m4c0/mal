#pragma once

#include "m4c0/parser/numeric.hpp"
#include "m4c0/parser/str.hpp"

namespace llvm {
  class Value;
};
namespace mal::parser {
  using namespace m4c0::parser;
  using type = llvm::Value *;
}
namespace mal::parser {
  static constexpr const auto space_char = skip(match_any_of(" \r\n\t,"));

  static constexpr const auto opt_space = many(space_char);
  static constexpr const auto space = at_least_one(space_char);
}
