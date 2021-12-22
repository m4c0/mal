#pragma once

#include "reader.hpp"

#include <sstream>
#include <string>
#include <variant>

namespace mal {
  static std::string pr_str(const mal::type & v);

  struct printer {
    std::string operator()(const types::boolean & b) {
      return *b ? "true" : "false";
    }

    std::string operator()(const types::error & e) {
      return *e;
    }

    std::string operator()(const types::hashmap & h) {
      std::ostringstream os;
      os << "{";
      bool first = true;
      for (const auto & kv : *h) {
        if (!first) {
          os << " ";
        }
        first = false;

        os << kv.first << " ";
        os << pr_str(kv.second);
      }
      os << "}";
      return os.str();
    }

    std::string operator()(const types::keyword & kw) {
      return { kw.value.begin(), kw.value.end() };
    }

    std::string operator()(const types::lambda & /*l*/) {
      return "#<function>";
    }

    std::string operator()(const types::list & h) {
      std::ostringstream os;
      os << "(";
      bool first = true;
      for (const auto & v : *h) {
        if (!first) {
          os << " ";
        }
        first = false;

        os << pr_str(v);
      }
      os << ")";
      return os.str();
    }

    std::string operator()(const types::nil & /*n*/) {
      return { "nil" };
    }

    std::string operator()(const types::number & n) {
      return std::to_string(*n);
    }

    std::string operator()(const types::symbol & sym) {
      return { sym.value.begin(), sym.value.end() };
    }

    std::string operator()(const types::string & s) {
      std::ostringstream os;
      os << '"';
      for (char c : **s) {
        switch (c) {
        case '"':
          os << "\\\"";
          break;
        case '\n':
          os << "\\n";
          break;
        case '\r':
          os << "\\r";
          break;
        case '\\':
          os << "\\\\";
          break;
        default:
          os << c;
          break;
        }
      }
      os << '"';
      return os.str();
    }

    std::string operator()(const types::vector & h) {
      std::ostringstream os;
      os << "[";
      bool first = true;
      for (const auto & v : *h) {
        if (!first) {
          os << " ";
        }
        first = false;

        os << pr_str(v);
      }
      os << "]";
      return os.str();
    }
  };

  static std::string pr_str(const mal::type & v) {
    return v.visit(printer {});
  }
}
