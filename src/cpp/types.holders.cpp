#include "types.holders.hpp"
#include "types.hpp"

using namespace mal::types::details;

void mal::types::details::push_back(std::vector<mal::type> & v, const mal::type & t) noexcept {
  v.push_back(t);
}
void mal::types::details::push_back(std::vector<mal::type> & v, std::span<const mal::type> t) noexcept {
  std::copy(t.begin(), t.end(), std::back_inserter(v));
}

static const mal::type oob {};
const mal::type & mal::types::details::oob() noexcept {
  return ::oob;
}
