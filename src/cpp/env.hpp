#pragma once

#include "types.hpp"

#include <unordered_map>
#include <utility>

namespace mal {
  class env {
    std::unordered_map<std::string, type> m_data {};
    std::shared_ptr<const env> m_outer {};

  public:
    env() = default;
    explicit env(std::shared_ptr<const env> outer) noexcept : m_outer(std::move(outer)) {
    }

    [[nodiscard]] type get(const std::string & key) const noexcept {
      auto it = m_data.find(key);
      if (it != m_data.end()) return it->second;
      if (m_outer == nullptr) return types::error { key + " not found" };
      return m_outer->get(key);
    }

    void set(const std::string & key, type val) noexcept {
      m_data[key] = std::move(val);
    }

    [[nodiscard]] type lookup(types::symbol s) const noexcept {
      return get(std::string { s.value.begin(), s.value.end() });
    }
  };
}
