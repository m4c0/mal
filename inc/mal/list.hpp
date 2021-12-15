#pragma once

#include <string>
#include <vector>

namespace mal {
  class list {
    std::vector<std::string> m_data {};

  public:
    using iterator = decltype(m_data)::const_iterator;

    list() = default;
    ~list() noexcept = default;

    list(const list &) = delete;
    list(list &&) noexcept = default;
    list & operator=(const list &) = delete;
    list & operator=(list &&) noexcept = default;

    list operator+(const char * token) noexcept {
      m_data.emplace_back(token);
      return std::move(*this);
    }

    [[nodiscard]] auto begin() const noexcept {
      return m_data.begin();
    }
    [[nodiscard]] auto end() const noexcept {
      return m_data.end();
    }
  };
}
