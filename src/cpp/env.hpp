#pragma once

#include "types.hpp"

#include <unordered_map>

namespace mal {
  class env {
    std::unordered_map<std::string, mal::types::lambda> m_data {};

  public:
    void emplace(const std::string & key, mal::types::lambda l) noexcept {
      m_data.emplace(key, l);
    }

    [[nodiscard]] mal::type lookup(mal::types::symbol s) const noexcept {
      auto it = m_data.find(std::string { s.value.begin(), s.value.end() });
      if (it == m_data.end()) {
        return mal::types::error { "Symbol not found" };
      }
      return it->second;
    }
  };
}
