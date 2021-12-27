#pragma once

#include "mal/parser.hpp"

#include <memory>
#include <string>
#include <utility>

namespace mal::types::details {
  template<typename Tp>
  class holder {
    Tp m_v;

  public:
    constexpr explicit holder(Tp v) noexcept : m_v(std::move(v)) {
    }

    [[nodiscard]] constexpr const Tp & operator*() const noexcept {
      return m_v;
    }
  };

  template<typename Tp>
  class heavy_holder {
    std::shared_ptr<Tp> m_v;

  public:
    explicit heavy_holder(Tp v) : m_v { std::make_shared<Tp>(std::move(v)) } {
    }

    [[nodiscard]] constexpr const Tp & operator*() const noexcept {
      return *m_v;
    }

    [[nodiscard]] const Tp & reset(const Tp & v) const noexcept {
      *m_v = v;
      return v;
    }
  };

  template<typename Tp>
  class token_holder {
    std::string m_v;

  public:
    constexpr explicit token_holder(parser::token<Tp> v) noexcept : m_v { v.value.begin(), v.value.length() } {
    }

    [[nodiscard]] constexpr const std::string & operator*() const noexcept {
      return m_v;
    }
  };

  template<typename Tp>
  [[nodiscard]] static bool operator==(const holder<Tp> & a, const holder<Tp> & b) noexcept {
    return *a == *b;
  }
  template<typename Tp>
  [[nodiscard]] static bool operator==(const heavy_holder<Tp> & a, const heavy_holder<Tp> & b) noexcept {
    return *a == *b;
  }
  template<typename Tp>
  [[nodiscard]] static bool operator==(const token_holder<Tp> & a, const token_holder<Tp> & b) noexcept {
    return *a == *b;
  }
}
