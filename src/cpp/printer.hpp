#pragma once

#include "reader.hpp"

#include <sstream>
#include <string>
#include <variant>

namespace mal {
  static std::string pr_str(const mal::type & v, bool readably);

  class printer {
    bool m_readably;

  public:
    constexpr explicit printer(bool r) : m_readably { r } {
    }

    std::string operator()(const types::boolean & b) const {
      return *b ? "true" : "false";
    }

    std::string operator()(const types::error & e) const {
      return *e;
    }

    std::string operator()(const types::hashmap & h) const {
      std::ostringstream os;
      os << "{";
      bool first = true;
      for (const auto & kv : *h) {
        if (!first) {
          os << " ";
        }
        first = false;

        os << kv.first << " ";
        os << pr_str(kv.second, m_readably);
      }
      os << "}";
      return os.str();
    }

    std::string operator()(const types::keyword & kw) const {
      return { kw.value.begin(), kw.value.end() };
    }

    std::string operator()(const types::lambda & /*l*/) const {
      return "#<function>";
    }

    std::string operator()(const types::list & h) const {
      std::ostringstream os;
      os << "(";
      bool first = true;
      for (const auto & v : *h) {
        if (!first) {
          os << " ";
        }
        first = false;

        os << pr_str(v, m_readably);
      }
      os << ")";
      return os.str();
    }

    std::string operator()(const types::nil & /*n*/) const {
      return { "nil" };
    }

    std::string operator()(const types::number & n) const {
      return std::to_string(*n);
    }

    std::string operator()(const types::symbol & sym) const {
      return { sym.value.begin(), sym.value.end() };
    }

    std::string operator()(const types::string & s) const {
      if (m_readably) return **s;

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

    std::string operator()(const types::vector & h) const {
      std::ostringstream os;
      os << "[";
      bool first = true;
      for (const auto & v : *h) {
        if (!first) {
          os << " ";
        }
        first = false;

        os << pr_str(v, m_readably);
      }
      os << "]";
      return os.str();
    }
  };

  static std::string pr_str(const mal::type & v, bool readably = false) {
    return v.visit(printer { readably });
  }
}
