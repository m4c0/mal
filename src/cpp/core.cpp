#include "core.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace mal::core::details {
  void pr_str(std::ostream & os, std::span<const type> args, const std::string & sep, bool readably) noexcept {
    bool first = true;
    for (const auto & a : args) {
      if (!first) os << sep;
      os << mal::pr_str(a, readably);
      first = false;
    }
  }

  type cout_pr_str(std::span<const type> args, bool readably) noexcept {
    details::pr_str(std::cout, args, " ", readably);
    std::cout << "\n";
    return types::nil {};
  }

  type prn(std::span<const type> args) noexcept {
    return cout_pr_str(args, true);
  }
  type println(std::span<const type> args) noexcept {
    return cout_pr_str(args, false);
  }
  type pr_str(std::span<const type> args) noexcept {
    std::ostringstream os;
    details::pr_str(os, args, " ", true);
    return types::string { os.str() };
  }
  type str(std::span<const type> args) noexcept {
    std::ostringstream os;
    details::pr_str(os, args, "", false);
    return types::string { os.str() };
  }

  type slurp(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("Can only slurp a single file");

    std::ifstream in { args[0].to_string() };
    if (!in) return err("Failure to slurp file");

    std::ostringstream os {};
    os << in.rdbuf();
    return types::string { os.str() };
  }
}
