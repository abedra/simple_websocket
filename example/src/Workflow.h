#pragma once

#include "Variant.h"
#include "PocoWrapper.h"
#include "ExampleWebSocket.h"
#include "ExampleFrameParser.h"

template<class L, class R, class A>
struct Workflow final {
  explicit Workflow(const bool &continueRunning) : continueRunning_(continueRunning) {}

  [[nodiscard]]
  Either<std::string, std::monostate> start(Poco::Net::WebSocket &pocoWebSocket, int frameSize) const {
    ExampleFrameParser frameParser;
    SimpleWebSocket::MessageParser<std::string> parser{std::make_unique<ExampleFrameParser>(frameParser)};
    PocoWrapper webSocket{pocoWebSocket, frameSize};
    int flags;

    try {
      while (continueRunning_) {
        std::span<char> result = webSocket.receive(flags);
        SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, result.data(), static_cast<int>(result.size()));
        std::string parsed = parser.parse(message);
        std::cout << parsed << std::endl;
      }
    } catch (const std::exception &e) {
      return Either<std::string, std::monostate>::left(e.what());
    }

    return Either<std::string, std::monostate>::right(std::monostate());
  }

  Either<L, R> runOnce(ExampleWebSocket &exampleWebSocket) {
    std::variant<std::string, Poco::Net::WebSocket> maybeWebSocket = exampleWebSocket.buildWebSocket();

    return std::visit(visitor{
        [](const std::string &error) {
          return Either<std::string, std::monostate>::left(error);
        },
        [this, &exampleWebSocket](Poco::Net::WebSocket &webSocket) {
          return start(webSocket, exampleWebSocket.frameSize());
        }
    }, maybeWebSocket);
  }

  void runUntil(const std::function<bool(const Either<L, R>& result)> &cancelPredicate,
                const std::function<void(const L &l)> &recoveryFn,
                A &executionContext) {
    Either<L, R> workflowResult = Workflow::runOnce(executionContext);
    workflowResult.template match<void>(recoveryFn, [](const R &r){});

    while (!cancelPredicate(workflowResult)) {
      workflowResult = Workflow::runOnce(executionContext);
      workflowResult.template match<void>(recoveryFn, [](const R &r){});
    }
  }

private:
  const bool& continueRunning_;
};