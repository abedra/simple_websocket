#pragma once

#include <variant>

template<class... As> struct visitor : As... { using As::operator()...; };
template<class... As> visitor(As...) -> visitor<As...>;

template <class L, class R>
struct Either final {
  explicit Either(std::variant<L, R> value) : value_(value) {}

  static Either<L, R> left(const L &l) {
    return Either(l);
  }

  static Either<L, R> right(const R &r) {
    return Either(r);
  }

  [[nodiscard]]
  bool isLeft() const {
    return std::holds_alternative<L>(value_);
  }

  [[nodiscard]]
  bool isRight() const {
    return std::holds_alternative<R>(value_);
  }

  template<class A>
  A match(std::function<A(const L &l)> leftFn, std::function<A(const R &r)> rightFn) {
    return std::visit(visitor{
      [&leftFn](const L &l){ leftFn(l); },
      [&rightFn](const R &r){ rightFn(r); }
    }, value_);
  }
private:
  std::variant<L, R> value_;
};