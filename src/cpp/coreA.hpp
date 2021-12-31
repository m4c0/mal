#pragma once

#include "core9.hpp"
#include "printer.hpp"

#include <chrono>
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
  static type time(std::span<const type> /*args*/) noexcept {
    auto epoch = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    return types::number { static_cast<int>(millis) };
  }
  static type is_fn(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::lambda>() };
  }
  static type is_number(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::number>() };
  }
  static type is_string(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::string>() };
  }
  static type fail(std::span<const type> /*args*/) noexcept {
    return err("not implemented");
  }

  static void setup_stepA_funcs(auto & e) noexcept {
    setup_step9_funcs(e);

    e->set("readline", types::lambda { readline });
    e->set("*host-language*", types::string { "c++" });

    e->set("time-ms", types::lambda { time });
    e->set("meta", types::lambda { fail });
    e->set("with-meta", types::lambda { fail });
    e->set("fn?", types::lambda { is_fn });
    e->set("string?", types::lambda { is_string });
    e->set("number?", types::lambda { is_number });
    e->set("seq", types::lambda { fail });
    e->set("conj", types::lambda { fail });
  }
}
