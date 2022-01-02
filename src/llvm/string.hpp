#pragma once

#include <cstddef>
#include <string_view>

namespace mal {
  class string {
    char * m_data {};
    size_t m_len {};
    size_t * m_mark {};

  public:
    constexpr string() noexcept = default;
    string(const char * data, size_t len) noexcept;
    explicit string(std::string_view o) noexcept : string { o.data(), o.size() } {
    }

    void mark(size_t gen) const noexcept {
      *m_mark = gen;
    }

    static void sweep(size_t cur_gen) noexcept;
  };
}
