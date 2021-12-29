#pragma once

#include <string>

namespace mal::types {
  class type;
}
namespace mal {
  [[nodiscard]] std::string pr_str(const types::type & v, bool readably = true);
}
