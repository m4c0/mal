#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace mal {
  template<typename T>
  class container {
    T m_data {};

  protected:
    [[nodiscard]] constexpr auto & data() noexcept {
      return m_data;
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

    [[nodiscard]] const auto & peek() const noexcept {
      return m_data;
    }
    [[nodiscard]] auto take() noexcept {
      return std::move(m_data);
    }
  };
  template<typename T>
  struct list : public container<std::vector<T>> {
    list operator+(T t) noexcept {
      this->data().emplace_back(std::move(t));
      return std::move(*this);
    }
  };

  template<typename T>
  struct hashmap_entry {
    std::string key;
    T value;
  };
  template<typename T>
  struct hashmap : public container<std::unordered_map<std::string, T>> {
    hashmap operator+(hashmap_entry<T> t) noexcept {
      this->data().emplace(t.key, std::move(t.value));
      return std::move(*this);
    }
  };

  template<typename T>
  struct vector : public container<std::vector<T>> {
    vector operator+(T t) noexcept {
      this->data().emplace_back(std::move(t));
      return std::move(*this);
    }
  };
}
