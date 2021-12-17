#pragma once

#include <string>

namespace mal {
  class str {
    std::string m_value {};

  public:
    str() = default;
    explicit str(std::string s) : m_value(std::move(s)) {
    }
    ~str() = default;

    str(const str &) = delete;
    str(str &&) noexcept = default;
    str & operator=(const str &) = delete;
    str & operator=(str &&) noexcept = default;

    [[nodiscard]] constexpr auto & operator*() const noexcept {
      return m_value;
    }

    auto operator+(char c) noexcept {
      m_value += c;
      return std::move(*this);
    }
  };
}
