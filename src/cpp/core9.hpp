#pragma once

#include "core8.hpp"
#include "printer.hpp"
#include "types.hpp"

#include <algorithm>
#include <iterator>
#include <unordered_map>

namespace mal::core {
  static type throw_(std::span<const type> args) noexcept {
    return types::error { args[0] };
  }
  static types::details::lambda_ret_t apply(std::span<const type> args, const std::shared_ptr<env> & e) noexcept {
    if (args.size() < 2) return { {}, err("apply requires at least two arguments") };
    if (!args[0].is<types::lambda>()) return { {}, err("apply require a function as first argument") };
    if (!args.back().is_iterable()) return { {}, err("apply require a list/vector as last argument") };

    const auto & fn = *args[0].as<types::lambda>();
    const auto fn_args = types::list { args.subspan(1, args.size() - 2), args.back().to_iterable() };
    return fn(*fn_args, e);
  }
  static types::details::lambda_ret_t map(std::span<const type> args, const std::shared_ptr<env> & e) noexcept {
    if (args.size() != 2) return { {}, err("map requires at two arguments") };
    if (!args[0].is<types::lambda>()) return { {}, err("map require a function as first argument") };
    if (!args[1].is_iterable()) return { {}, err("map require a list/vector as last argument") };

    const auto & fn = *args[0].as<types::lambda>();

    auto values = args[1].to_iterable();

    std::vector<type> res;
    res.reserve(values.size());

    for (const auto & v : values) {
      auto fn_res = fn(std::span { &v, 1 }, e);
      auto new_v = EVAL(fn_res.t, fn_res.e);
      if (new_v.is_error()) return { {}, new_v };
      res.push_back(new_v);
    }
    return { {}, types::list { res } };
  }
  static type is_nil(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::nil>() };
  }
  static type is_true(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].to_boolean(false) };
  }
  static type is_false(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && !args[0].to_boolean(false) };
  }
  static type is_symbol(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::symbol>() };
  }
  static type is_keyword(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::keyword>() };
  }
  static type is_sequential(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is_iterable() };
  }
  static type is_vector(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::vector>() };
  }
  static type is_map(std::span<const type> args) noexcept {
    return types::boolean { args.size() == 1 && args[0].is<types::hashmap>() };
  }

  static type symbol(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("symbol requires a string argument");
    if (!args[0].is<types::string>()) return err("symbol requires a string argument");
    return types::symbol { args[0].to_string() };
  }
  static type keyword(std::span<const type> args) noexcept {
    if (args.size() != 1) return err("keyword requires a string argument");
    if (args[0].is<types::string>()) return types::keyword { std::string { ":" } + args[0].to_string() };
    if (args[0].is<types::keyword>()) return args[0];
    return err("keyword requires a string argument");
  }
  static type vector(std::span<const type> args) noexcept {
    return types::vector { args };
  }

  static type merge(std::unordered_map<std::string, type> map, std::span<const type> args) noexcept {
    for (int i = 0; i < args.size(); i += 2) {
      const auto & key = args[i];
      const auto & value = args[i + 1];

      if (key.is<types::string>()) {
        map[pr_str({ &key, 1 }).to_string()] = value;
        continue;
      }
      if (key.is<types::keyword>()) {
        map[*key.as<types::keyword>()] = value;
        continue;
      }
      return err("keys must be strings or keywords");
    }
    return types::hashmap { std::move(map) };
  }
  static type hashmap(std::span<const type> args) noexcept {
    if (args.size() % 2 == 1) return err("hash-map requires an even number of arguments");
    return merge({}, args);
  }
  static type assoc(std::span<const type> args) noexcept {
    if (args.size() % 2 == 0) return err("assoc requires a map plus an even number of arguments");
    if (!args[0].is<types::hashmap>()) return err("assoc requires a map as first argument");
    return merge(*args[0].as<types::hashmap>(), args.subspan(1));
  }

  static void setup_step9_funcs(auto rep, auto & e) noexcept {
    setup_step8_funcs(rep, e);

    e->set("throw", types::lambda { throw_ });
    e->set("apply", types::lambda { 0, apply });
    e->set("map", types::lambda { 0, map });

    e->set("nil?", types::lambda { is_nil });
    e->set("true?", types::lambda { is_true });
    e->set("false?", types::lambda { is_false });
    e->set("symbol?", types::lambda { is_symbol });

    e->set("symbol", types::lambda { symbol });
    e->set("keyword", types::lambda { keyword });
    e->set("keyword?", types::lambda { is_keyword });
    e->set("vector", types::lambda { vector });
    e->set("vector?", types::lambda { is_vector });
    e->set("sequential?", types::lambda { is_sequential });
    e->set("hash-map", types::lambda { hashmap });
    e->set("map?", types::lambda { is_map });
    e->set("assoc", types::lambda { assoc });
  }
}
