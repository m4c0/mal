#pragma once

#include "m4c0/parser/tokeniser.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace mal {
  class type {};

  using u_type = std::unique_ptr<type>;
}
namespace mal::types {
  class list : public type {
    std::vector<u_type> m_data;

  public:
    explicit constexpr list(auto l) : m_data { l.take() } {
    }
  };

  class number : public type {
    int m_value;

  public:
    explicit constexpr number(int n) : m_value(n) {
    }
  };

  class symbol : public type {
    std::string_view m_token;

  public:
    explicit constexpr symbol(m4c0::parser::token<void> t) : m_token({ t.value.begin(), t.value.length() }) {
    }
  };
}
