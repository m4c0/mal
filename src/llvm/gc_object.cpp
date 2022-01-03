#include "gc_object.hpp"

#include <vector>

static std::vector<size_t *> & gc_roots() noexcept {
  static constexpr const auto initial_capacity = 1000;
  static std::vector<size_t *> roots { initial_capacity };
  return roots;
}

static unsigned & gc_gen() noexcept {
  static unsigned g = 0;
  return g;
}

[[nodiscard]] void * operator new(size_t len, mal::gc /**/) {
  auto * res = static_cast<size_t *>(::operator new(len + sizeof(size_t)));
  gc_roots().emplace_back(res);
  return res + 1; // NOLINT
}
[[nodiscard]] void * operator new[](size_t len, mal::gc /**/) {
  return ::operator new (len, mal::gc {});
}
void operator delete(void * ptr, mal::gc /**/) {
  auto * alloc = static_cast<size_t *>(ptr);
  alloc--; // NOLINT

  auto & roots = gc_roots();
  roots.erase(std::find(roots.begin(), roots.end(), alloc));

  ::operator delete(alloc);
}
void operator delete[](void * ptr, mal::gc /**/) {
  ::operator delete (ptr, mal::gc {});
}

void mal::gc_mark(void * ptr) noexcept {
}
void mal::gc_sweep() noexcept {
  auto & cur_gen = gc_gen();
  auto & roots = gc_roots();
  for (auto it = roots.begin(); it != roots.end();) {
    if (**it < cur_gen) {
      ::operator delete (*it + 1, mal::gc {});
    } else {
      ++it;
    }
  }
  cur_gen++;
}
