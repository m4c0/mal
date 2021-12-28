#pragma once

#include "log.hpp"
#include "mal/parser.hpp"

#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace mal::types {
  class type;
}
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
  struct token_holder : heavy_holder<std::string> {
    using heavy_holder::heavy_holder;
    explicit token_holder(parser::token<Tp> v) noexcept
      : heavy_holder { std::string { v.value.begin(), v.value.length() } } {
    }
  };

  template<typename Tp>
  concept type_or_list =
      std::is_same_v<std::decay_t<Tp>, type> || std::is_same_v<std::decay_t<Tp>, std::span<const type>>;

  void push_back(std::vector<type> & v, const type & t) noexcept;
  void push_back(std::vector<type> & v, std::span<const type> t) noexcept;
  const type & oob() noexcept;

  template<typename Self>
  class vector_holder : public heavy_holder<std::vector<type>> {
    using parent_t = heavy_holder<std::vector<type>>;

    template<type_or_list... Args>
    static std::vector<type> build(Args &&... args) {
      std::vector<type> res {};
      res.reserve(sizeof...(Args));
      (push_back(res, args), ...);
      return res;
    }

  public:
    using parent_t::parent_t;

    template<type_or_list... Args>
    explicit vector_holder(Args &&... args) : parent_t { build(std::forward<Args>(args)...) } {
    }

    [[nodiscard]] const type & at(auto idx) const noexcept {
      if (idx >= size()) {
        log::debug() << "OOB accessing element at " << idx << "\n";
        return oob();
      }
      return (**this).at(idx);
    }
    [[nodiscard]] bool empty() const noexcept {
      return (**this).empty();
    }
    [[nodiscard]] auto size() const noexcept {
      return (**this).size();
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
