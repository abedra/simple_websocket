#pragma once

#include "Variant.h"

template<class A>
struct WorkflowResult final {
  explicit WorkflowResult(const std::monostate &unit) : value_(unit) { }
  explicit WorkflowResult(const A &a) : value_(a) { }
  explicit WorkflowResult(const std::variant<A, std::monostate> &value) : value_(value) { }

  [[nodiscard]] bool operator==(const WorkflowResult &rhs) const {
    return value_ == rhs.value_;
  }

  [[nodiscard]] bool operator!=(const WorkflowResult &rhs) const {
    return rhs != *this;
  }

  [[nodiscard]] const std::variant<A, std::monostate> &value() const {
    return value_;
  }

  [[nodiscard]] bool complete() const {
    return std::holds_alternative<std::monostate>(value_);
  }

  template<class R>
  R match(const std::function<R(const A &a)> aFn, const std::function<R(const std::monostate &unit)> uFn) {
    return std::visit(visitor{
      [&aFn](const A &a) { return aFn(a); },
      [&uFn](const std::monostate &u){ return uFn(u); }
    }, value_);
  }

private:
  std::variant<A, std::monostate> value_;
};