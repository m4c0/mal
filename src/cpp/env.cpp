#include "env.hpp"

#include <sstream>

using namespace mal;

type env::err_not_found(const std::string & key) noexcept {
  std::ostringstream os;
  os << "'" << key << "' not found";
  return err(os.str());
}
