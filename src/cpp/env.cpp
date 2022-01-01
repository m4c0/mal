#include "env.hpp"

#include <sstream>

using namespace mal;

type env::err_not_found(const std::string & key) noexcept {
  std::ostringstream os;
  os << "'" << key << "' not found";
  return err(os.str());
}
std::shared_ptr<env> env::make() noexcept {
  return std::shared_ptr<env>(new env {});
}
std::shared_ptr<env> env::extend() noexcept {
  return std::shared_ptr<env>(new env { shared_from_this() });
}
