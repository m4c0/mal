#pragma once

#include <string>
#include <vector>

namespace mal {
  template<typename T>
  class container {
    std::vector<T> m_data {};

  protected:
    void emplace_back(T t) noexcept {
      m_data.emplace_back(std::move(t));
    }

  public:
    using iterator = typename decltype(m_data)::const_iterator;

    container() = default;
    ~container() noexcept = default;

    container(const container &) = delete;
    container(container &&) noexcept = default;
    container & operator=(const container &) = delete;
    container & operator=(container &&) noexcept = default;

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
  template<typename T>
  struct list : public container<T> {
    list operator+(T t) noexcept {
      container<T>::emplace_back(std::move(t));
      return std::move(*this);
    }
  };
  template<typename T>
  struct vector : public container<T> {
    vector operator+(T t) noexcept {
      container<T>::emplace_back(std::move(t));
      return std::move(*this);
    }
  };
}
