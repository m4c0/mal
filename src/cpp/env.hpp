#pragma once

#include "types.hpp"

#include <memory>
#include <unordered_map>

namespace mal {
  class env : public std::enable_shared_from_this<env> {
    std::unordered_map<std::string, type> m_data {};
    std::shared_ptr<const env> m_outer {};

    static type err_not_found(const std::string & key) noexcept;

    env() = default;
    explicit env(std::shared_ptr<const env> outer) noexcept : m_outer(std::move(outer)) {
    }

  public:
    [[nodiscard]] type get(const std::string & key) const noexcept {
      auto it = m_data.find(key);
      if (it != m_data.end()) return it->second;
      if (m_outer != nullptr) return m_outer->get(key);
      return err_not_found(key);
    }

    void set(const std::string & key, type val) noexcept {
      m_data[key] = std::move(val);
    }

    [[nodiscard]] type lookup(const std::string & key) const noexcept {
      return get(key);
    }

    [[nodiscard]] static std::shared_ptr<env> make() noexcept;
    [[nodiscard]] std::shared_ptr<env> extend() noexcept;
  };
  using senv = const std::shared_ptr<env> &;
}
