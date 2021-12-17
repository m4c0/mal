#pragma once

#include <string>
#include <vector>

namespace mal {
  template<typename T>
  class list {
    std::vector<T> m_data {};

  public:
    using iterator = typename decltype(m_data)::const_iterator;

    list() = default;
    ~list() noexcept = default;

    list(const list &) = delete;
    list(list &&) noexcept = default;
    list & operator=(const list &) = delete;
    list & operator=(list &&) noexcept = default;

    list operator+(T t) noexcept {
      m_data.emplace_back(std::move(t));
      return std::move(*this);
    }

    [[nodiscard]] auto begin() const noexcept {
      return m_data.begin();
    }
    [[nodiscard]] auto end() const noexcept {
      return m_data.end();
    }

    [[nodiscard]] auto take() noexcept {
      return std::move(m_data);
    }
  };
}
