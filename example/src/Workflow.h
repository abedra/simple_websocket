#pragma once

#include "Variant.h"
#include "PocoWrapper.h"
#include "ExampleWebSocket.h"
#include "ExampleFrameParser.h"

template<class Failure, class Success, class ExecutionContext>
struct Workflow final {
  explicit Workflow(ExecutionContext &executionContext,
                    const std::function<Either<Failure, Success>(ExecutionContext &)> &runFn,
                    const std::function<void(const Success &)> &successFn,
                    const std::function<void(const Failure &)> &recoveryFn)
    : executionContext_(executionContext)
    , runFn_(runFn)
    , successFn_(successFn)
    , recoveryFn_(recoveryFn)
  { }

  void runUntilCancelled() {
    Either<Failure, Success> workflowResult = runFn_(executionContext_);
    workflowResult.template match<void>(recoveryFn_, successFn_);

    while (!workflowResult.isRight()) {
      workflowResult = runFn_(executionContext_);
      workflowResult.template match<void>(recoveryFn_, successFn_);
    }
  }

private:
  ExecutionContext &executionContext_;
  const std::function<Either<Failure, Success>(ExecutionContext &)> runFn_;
  const std::function<void(const Success &)> successFn_;
  const std::function<void(const Failure &)> recoveryFn_;
};