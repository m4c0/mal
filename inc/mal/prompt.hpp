#pragma once

#include <iostream>
#include <string>

namespace mal::prompt {
  static auto & read_user_input(std::string & line) {
    std::cout << "user> ";
    return std::getline(std::cin, line);
  }

  template<typename Fn>
  static void loop(Fn && rep) {
    while (true) {
      for (std::string line; read_user_input(line);) {
        std::cout << rep(line) << "\n";
      }
    }
  }
}
