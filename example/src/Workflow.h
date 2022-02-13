#pragma once

#include <utility>

#include "Variant.h"
#include "PocoWrapper.h"
#include "ExampleFrameParser.h"
#include "WorkflowResult.h"

template<class Failure>
struct Workflow final {
  explicit Workflow(const std::function<WorkflowResult<Failure>()> &runFn,
                    std::function<void(const std::monostate &)> successFn,
                    const std::function<void(const Failure &)> &recoveryFn)
    : runFn_(runFn)
    , successFn_(std::move(successFn))
    , recoveryFn_(recoveryFn)
  { }

  void runUntilCancelled() {
    WorkflowResult<Failure> workflowResult = runFn_();
    workflowResult.template match<void>(recoveryFn_, successFn_);

    while (!workflowResult.complete()) {
      workflowResult = runFn_();
      workflowResult.template match<void>(recoveryFn_, successFn_);
    }
  }

private:
  const std::function<WorkflowResult<Failure>()> runFn_;
  const std::function<void(const std::monostate &)> successFn_;
  const std::function<void(const Failure &)> recoveryFn_;
};