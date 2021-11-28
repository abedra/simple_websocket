#pragma once

#include <variant>

#include "Variant.h"
#include "Workflow.h"
#include "PocoWrapper.h"
#include "ExampleWebSocket.h"

struct WebSocketWorkflow final {
  explicit WebSocketWorkflow(ExampleWebSocket &executionContext, const bool &continueRunning)
    : workflow_{
        executionContext,
        [this](ExampleWebSocket &executionContext) { return run(executionContext); },
        [](const std::monostate &_){},
        [](const std::string &failure) {
          std::cout << failure << std::endl;
          sleep(1);
        }
      }
    , continueRunning_(continueRunning)
  { }

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

  Either<std::string, std::monostate> run(ExampleWebSocket &executionContext) {
    std::variant<std::string, Poco::Net::WebSocket> maybeWebSocket = executionContext.buildWebSocket();

    return std::visit(visitor{
        [](const std::string &error) {
          return Either<std::string, std::monostate>::left(error);
        },
        [this, &executionContext](Poco::Net::WebSocket &webSocket) {
          return start(webSocket, executionContext.frameSize());
        }
    }, maybeWebSocket);
  }

  void runUntilCancelled() {
    return workflow_.runUntilCancelled();
  }

private:
  Workflow<std::string, std::monostate, ExampleWebSocket> workflow_;
  const bool &continueRunning_;
};