#include "mal/prompt.hpp"
#include "mal/reader.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <string>
#include <type_traits>
#include <utility>

struct context {
  llvm::LLVMContext ctx {};
  std::unique_ptr<llvm::Module> m = std::make_unique<llvm::Module>("mal", ctx);

  llvm::Type * i8p { llvm::Type::getInt8PtrTy(ctx) };

  llvm::IRBuilder<> builder { ctx };
};

class no_copy_str {
  std::string m_value {};

public:
  no_copy_str() = default;
  explicit no_copy_str(std::string s) : m_value(std::move(s)) {
  }
  ~no_copy_str() = default;

  no_copy_str(const no_copy_str &) = delete;
  no_copy_str(no_copy_str &&) noexcept = default;
  no_copy_str & operator=(const no_copy_str &) = delete;
  no_copy_str & operator=(no_copy_str &&) noexcept = default;

  [[nodiscard]] constexpr auto & operator*() const noexcept {
    return m_value;
  }
};
struct printer {
  using result_t = no_copy_str;

  static result_t print_container(const mal::container<result_t> & items, char l, char r) noexcept {
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

std::string rep(const std::string & s) {
  auto c = std::make_unique<context>();

  auto * bb = llvm::BasicBlock::Create(c->ctx, "entry");
  c->builder.SetInsertPoint(bb);

  llvm::FunctionType * fn_tp { llvm::FunctionType::get(c->i8p, false) };
  llvm::Function * fn { llvm::Function::Create(fn_tp, llvm::Function::InternalLinkage, "", *c->m) };
  c->builder.GetInsertBlock()->insertInto(fn);

  auto res = mal::read_str(s, printer {});
  if (!res) {
    llvm::errs() << res.takeError() << "\n";
    return "ERROR";
  }
  c->builder.CreateRet(c->builder.CreateGlobalStringPtr(*res.get()));

  if (llvm::verifyModule(*c->m, &llvm::errs())) return "Failed to generate valid code";

  auto * ee = llvm::EngineBuilder { std::move(c->m) }.create();
  if (ee == nullptr) return "Failure creating JIT engine";

  return static_cast<const char *>(ee->runFunction(fn, {}).PointerVal); // NOLINT
}

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  mal::prompt::loop(rep);

  llvm::llvm_shutdown();
}
