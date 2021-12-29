#pragma once

#include "core9.hpp"
#include "printer.hpp"

#include <string>

namespace mal::core {
  static type readline(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("readline requires a string");
    if (!args[0].is<types::string>()) return err("readline requires a string");

    std::cout << mal::pr_str(args[0], false);

    std::string line;
    std::getline(std::cin, line);
    if (!std::cin) return types::nil {};
    return types::string { line };
  }
  static type fail(std::span<const type> /*args*/) noexcept {
    return err("not implemented");
  }

  static void setup_stepA_funcs(auto rep, auto & e) noexcept {
    setup_step9_funcs(rep, e);

    e->set("readline", types::lambda { readline });
    e->set("*host-language*", types::string { "c++" });

    e->set("time-ms", types::lambda { fail });
    e->set("meta", types::lambda { fail });
    e->set("with-meta", types::lambda { fail });
    e->set("fn?", types::lambda { fail });
    e->set("string?", types::lambda { fail });
    e->set("number?", types::lambda { fail });
    e->set("seq", types::lambda { fail });
    e->set("conj", types::lambda { fail });
  }
}
