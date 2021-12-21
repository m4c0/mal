#include "mal/prompt.hpp"
#include "mal/reader.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <string>
#include <type_traits>
#include <utility>

struct printer {
  using result_t = mal::str;

  static result_t print_container(const mal::container<std::vector<result_t>> & items, char l, char r) noexcept {
    std::string s;
    llvm::raw_string_ostream os { s };
    os << l;
    bool first = true;
    for (const auto & item : items) {
      if (!first) os << " ";
      first = false;
      os << *item;
    }
    os << r;
    return result_t { std::move(os.str()) };
  }
  result_t operator()(const mal::list<result_t> & items) const noexcept {
    return print_container(items, '(', ')');
  }
  result_t operator()(const mal::hashmap<result_t> & items) const noexcept {
    std::string s;
    llvm::raw_string_ostream os { s };
    os << '{';
    bool first = true;
    for (const auto & item : items) {
      if (!first) os << " ";
      first = false;
      os << item.first << " " << *item.second;
    }
    os << '}';
    return result_t { std::move(os.str()) };
  }
  result_t operator()(const mal::vector<result_t> & items) const noexcept {
    return print_container(items, '[', ']');
  }
  template<typename T>
  result_t operator()(mal::parser::token<T> t) const noexcept {
    return result_t { std::string { t.value.begin(), t.value.length() } };
  }
  result_t operator()(mal::str t) const noexcept {
    std::string s;
    llvm::raw_string_ostream os { s };
    os << '"';
    for (auto c : *t) {
      switch (c) {
      case '\r':
        os << "\\r";
        break;
      case '\n':
        os << "\\n";
        break;
      case '\\':
        os << "\\\\";
        break;
      case '\"':
        os << "\\\"";
        break;
      default:
        os << c;
        break;
      }
    }
    os << '"';
    return result_t { std::move(os.str()) };
  }
  result_t operator()(bool b) const noexcept {
    return result_t { b ? "true" : "false" };
  }
  result_t operator()(const mal::parser::nil & /*v*/) const noexcept {
    return result_t { "nil" };
  }
  result_t operator()(auto i) const noexcept {
    return result_t { std::to_string(i) };
  }
};

llvm::Expected<llvm::Value *> rep(mal::context * c, const std::string & s) {
  auto res = mal::parser::read_str(s, printer {});
  if (!res) {
    return llvm::createStringError(llvm::inconvertibleErrorCode(), "EOF");
  }
  return c->builder.CreateGlobalStringPtr(**res, "", 0, c->m.get());
}

int main() {
  mal::prompt::loop(rep);
}
