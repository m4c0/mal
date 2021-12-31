#pragma once

#include "core.hpp"
#include "eval.hpp"
#include "printer.hpp"
#include "reader.hpp"

namespace mal {
  static auto READ(auto in) {
    return read_str(in);
  }
  static auto PRINT(auto in) {
    if (in.is_error()) return std::string { "Error: " } + pr_str(in);
    return pr_str(in);
  }

  static auto rep(const std::string & in, senv e) {
    return PRINT(EVAL(READ(in), e));
  }

  static auto readline(std::string & line) {
    std::cout << "user> ";
    return static_cast<bool>(std::getline(std::cin, line));
  }

  static void run(senv e) {
    std::string line;
    while (readline(line)) {
      std::cout << rep(line, e) << "\n";
    }
  }
  static void run(int argc, char ** argv, senv e) {
    if (core::setup_argv(argc, argv, rep, e)) {
      return;
    }
    run(e);
  }
  static void run_with_banner(int argc, char ** argv, senv e) {
    if (core::setup_argv(argc, argv, rep, e)) {
      return;
    }
    rep(R"--((println (str "Mal [" *host-language* "]")))--", e);
    run(e);
  }
}
