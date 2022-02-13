#pragma once

#include <utility>
#include <ostream>
#include <variant>
#include <functional>

#include <Poco/Types.h>

template<class... As> struct visitor : As... { using As::operator()...; };
template<class... As> visitor(As...) -> visitor<As...>;

struct ExecutionContext final {
  ExecutionContext(std::string host, unsigned short port, std::string uri)
      : host_(std::move(host))
      , port_(port)
      , uri_(std::move(uri))
  { }

  [[nodiscard]] const std::string &host() const {
    return host_;
  }

  [[nodiscard]] Poco::UInt16 port() const {
    return port_;
  }

  [[nodiscard]] const std::string &uri() const {
    return uri_;
  }

private:
  std::string host_;
  Poco::UInt16 port_;
  std::string uri_;
};

struct Failure final {
  explicit Failure(std::string value) : value_(std::move(value)) { }

  friend std::ostream &operator<<(std::ostream &os, const Failure &failure) {
    os << failure.value_;
    return os;
  }

  [[nodiscard]] const std::string &value() const {
    return value_;
  }

private:
  std::string value_;
};

struct WorkflowResult final {
  explicit WorkflowResult(const std::monostate &unit) : value_(unit) { }
  explicit WorkflowResult(const Failure &f) : value_(f) { }
  explicit WorkflowResult(std::variant<Failure, std::monostate> value) : value_(std::move(value)) { }

  [[nodiscard]] const std::variant<Failure, std::monostate> &value() const {
    return value_;
  }

  [[nodiscard]] bool complete() const {
    return std::holds_alternative<std::monostate>(value_);
  }

  template<class R>
  [[nodiscard]] R match(const std::function<R(const Failure &a)> aFn, const std::function<R(const std::monostate &unit)> uFn) {
    return std::visit(visitor{
        [&aFn](const Failure &a) { return aFn(a); },
        [&uFn](const std::monostate &u){ return uFn(u); }
    }, value_);
  }

private:
  std::variant<Failure, std::monostate> value_;
};

struct Workflow final {
  explicit Workflow(std::function<WorkflowResult()> runFn,
                    std::function<void(const std::monostate &)> successFn,
                    std::function<void(const Failure &)> recoveryFn)
      : runFn_(std::move(runFn))
      , successFn_(std::move(successFn))
      , recoveryFn_(std::move(recoveryFn))
  { }

  void runUntilCancelled() {
    WorkflowResult workflowResult = runFn_();
    workflowResult.template match<void>(recoveryFn_, successFn_);

    while (!workflowResult.complete()) {
      workflowResult = runFn_();
      workflowResult.template match<void>(recoveryFn_, successFn_);
    }
  }

private:
  const std::function<WorkflowResult()> runFn_;
  const std::function<void(const std::monostate &)> successFn_;
  const std::function<void(const Failure &)> recoveryFn_;
};