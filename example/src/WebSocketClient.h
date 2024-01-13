#pragma once

#include <simple_websocket.hpp>
#include "ExampleFrameParser.h"

template<int FRAME_SIZE>
struct WebSocketClient final {
  explicit WebSocketClient(SimpleWebSocket::ExecutionContext executionContext,
                           std::function<SimpleWebSocket::WorkflowResult(const std::variant<SimpleWebSocket::Failure, std::monostate>&)> resultFn)
    : executionContext_(std::move(executionContext))
    , resultFn_(resultFn)
  { }

   [[nodiscard]] SimpleWebSocket::WorkflowResult start(std::atomic<bool> &continueRunning) const {
    ExampleFrameParser frameParser;
    SimpleWebSocket::MessageParser<std::variant<std::monostate, std::string>> parser{
      std::make_unique<ExampleFrameParser>(frameParser)
    };

    int flags;

    try {
      SimpleWebSocket::Poco::Wrapper<FRAME_SIZE> delegate = SimpleWebSocket::Poco::wrapper<FRAME_SIZE>(
          executionContext_.host(),
          executionContext_.port(),
          executionContext_.uri()
      );

      while (continueRunning) {
        std::span<char> result = delegate.receive(flags);
        SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, result.data(), static_cast<int>(result.size()));
        const std::variant<std::monostate, std::string> &parseResult = parser.parse(message);
        if (std::holds_alternative<std::monostate>(parseResult)) {
          return resultFn_(SimpleWebSocket::Failure{"WebSocket connection closed"});
        }

        std::cout << std::get<std::string>(parseResult) << std::endl;
      }
    } catch (const std::exception &e) {
       return resultFn_(SimpleWebSocket::Failure{e.what()});
    }

     return resultFn_(std::monostate());
  }

private:
  SimpleWebSocket::ExecutionContext executionContext_;
  std::function<SimpleWebSocket::WorkflowResult(const std::variant<SimpleWebSocket::Failure, std::monostate>&)> resultFn_;
};