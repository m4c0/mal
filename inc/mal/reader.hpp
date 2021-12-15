#pragma once

#include "mal/parser.hpp"
#include "mal/types.hpp"

#include <llvm/Support/Error.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace mal {
  struct rec_form {
    parser::result<u_type> operator()(parser::input_t in) const noexcept;
  };

  static constexpr const auto read_atom = parser::atom;
  static constexpr const auto read_list = parser::list(rec_form {});
  static constexpr const auto read_form = parser::space & (read_list | read_atom);

  inline parser::result<u_type> rec_form::operator()(parser::input_t in) const noexcept {
    return read_form(in);
  }

  static llvm::Expected<mal::u_type> read_str(const std::string & s) {
    auto t = read_form({ s.data(), s.length() });
    return t % [](auto v) noexcept -> llvm::Expected<mal::u_type> {
      if constexpr (std::is_same_v<decltype(v), parser::input_t>) {
        return llvm::createStringError(llvm::inconvertibleErrorCode(), std::string_view { v.begin(), v.length() });
      } else {
        return v;
      }
    };
  }
}
