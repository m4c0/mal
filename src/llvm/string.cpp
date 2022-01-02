#include "string.hpp"

#include <cstring>
#include <vector>

using namespace mal;

struct allocation {
  char * ptr;
  size_t mark;
};
static std::vector<allocation> & gc_roots() noexcept {
  static constexpr const auto initial_capacity = 1000;
  static std::vector<allocation> roots { initial_capacity };
  return roots;
}
static allocation & alloc(size_t len) {
  char * res = static_cast<char *>(malloc(len)); // NOLINT
  return gc_roots().emplace_back(allocation { res });
}

string::string(const char * data, size_t len) noexcept : m_len { len } {
  auto & r = alloc(len);
  m_data = r.ptr;
  m_mark = &r.mark;

  strncpy(m_data, data, len);
}

void string::sweep(size_t cur_gen) noexcept {
  auto & roots = gc_roots();
  for (auto it = roots.begin(); it != roots.end();) {
    if (it->mark < cur_gen) {
      free(it->ptr); // NOLINT
    } else {
      ++it;
    }
  }
}
