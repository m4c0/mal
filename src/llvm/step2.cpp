#include "mal/env.hpp"
#include "mal/prompt.hpp"
#include "mal/reader.hpp"

#include <cstdio>
#include <iterator>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

extern "C" const char * mal_i2s(int i) {
  static char buf[1024];               // NOLINT
  snprintf(buf, sizeof(buf), "%d", i); // NOLINT
  return buf;                          // NOLINT
}

class eval_ast {
  using result_t = llvm::Value *;

  mal::context * m_c;
  mal::env * m_e;

public:
  constexpr eval_ast(mal::context * c, mal::env * e) : m_c(c), m_e(e) {
  }

  result_t operator()(const mal::list<result_t> & items) const noexcept {
    auto size = items.end() - items.begin();

    // TODO: new "empty list" or something
    if (size == 0) return nullptr;

    auto * v = *items.begin();
    if (v == nullptr) return nullptr; // TODO: "symbol not found"
    if (!llvm::isa<llvm::Function>(v)) return nullptr;

    auto * fn = dyn_cast<llvm::Function>(v);
    if (size - 1 != fn->arg_size()) return nullptr; // TODO: "mismatched arg size"

    std::vector<result_t> args {};
    std::copy(items.begin() + 1, items.end(), std::back_inserter(args));
    return m_c->builder.CreateCall(fn, args);
  }
  result_t operator()(const mal::hashmap<result_t> & items) const noexcept {
    return nullptr;
  }
  result_t operator()(const mal::vector<result_t> & items) const noexcept {
    return nullptr;
  }
  // keyword
  result_t operator()(mal::parser::token<mal::parser::kw> t) const noexcept {
    // TODO: introduce a custom type
    return m_c->builder.CreateGlobalStringPtr({ t.value.begin(), t.value.length() }, "", 0, m_c->m.get());
  }
  // symbol
  result_t operator()(mal::parser::token<void> t) const noexcept {
    return m_e->lookup({ t.value.begin(), t.value.length() });
  }
  // string (unencoded)
  result_t operator()(mal::str t) const noexcept {
    return m_c->builder.CreateGlobalStringPtr(*t, "", 0, m_c->m.get());
  }
  // boolean (duh)
  result_t operator()(bool b) const noexcept {
    return llvm::ConstantInt::getBool(m_c->ctx, b);
  }
  // nil (duh)
  result_t operator()(const mal::parser::nil & /*v*/) const noexcept {
    return llvm::ConstantTokenNone::get(m_c->ctx);
  }
  // numbers
  result_t operator()(int i) const noexcept {
    auto * i32 = llvm::IntegerType::getInt32Ty(m_c->ctx);
    return llvm::ConstantInt::getSigned(i32, i);
  }
};

llvm::Expected<llvm::Value *> rep(mal::context * c, const std::string & s) {
  mal::env e { *c };

  auto exp = mal::parser::read_str(s, eval_ast { c, &e });
  if (exp && *exp == nullptr) {
    return llvm::createStringError(llvm::inconvertibleErrorCode(), "Invalid symbol");
  }

  auto * res = *exp;
  auto * res_tp = res->getType();

  llvm::FunctionType * i2s_tp { llvm::FunctionType::get(c->i8p, { res_tp }, false) };
  llvm::Function * i2s_fn { llvm::Function::Create(i2s_tp, llvm::Function::ExternalLinkage, "mal_i2s", *c->m) };
  return c->builder.CreateCall(i2s_fn, { res });
}

int main() {
  mal::prompt::loop(rep);
}
