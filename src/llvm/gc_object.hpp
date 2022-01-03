#pragma once

#include <cstddef>

namespace mal {
  class gc {};

  void gc_mark(void * ptr) noexcept;
  void gc_sweep() noexcept;
}
[[nodiscard]] void * operator new(size_t len, mal::gc /**/);
[[nodiscard]] void * operator new[](size_t len, mal::gc /**/);
void operator delete(void * ptr, mal::gc /**/);
void operator delete[](void * ptr, mal::gc /**/);
