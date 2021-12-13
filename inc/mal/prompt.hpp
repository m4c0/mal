#pragma once

#include <iostream>
#include <string>
#include <type_traits>

namespace mal::prompt {
  static auto & read_user_input(std::string & line) {
    std::cout << "user> ";
    return std::getline(std::cin, line);
  }

  template<typename Fn>
  requires std::is_invocable_r_v<std::string, Fn, std::string>
  static void loop(Fn && rep) {
    while (std::cin) {
      for (std::string line; read_user_input(line);) {
        std::cout << rep(line) << "\n";
      }
    }
  }
}
