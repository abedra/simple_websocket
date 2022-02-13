#pragma once

#include "Types.h"
#include "ExampleFrameParser.h"
#include "PocoWrapper.h"

template<int FRAME_SIZE, class A>
struct WebSocketClient final {
  explicit WebSocketClient(ExecutionContext executionContext,
                           std::function<A(const std::variant<Failure, std::monostate>&)> resultFn)
    : executionContext_(std::move(executionContext))
    , resultFn_(resultFn)
  { }

   [[nodiscard]] A start(std::atomic<bool> &continueRunning) const {
    ExampleFrameParser frameParser;
    SimpleWebSocket::MessageParser<std::variant<std::monostate, std::string>> parser{
      std::make_unique<ExampleFrameParser>(frameParser)
    };

    int flags;

    try {
      PocoWrapper<FRAME_SIZE> delegate = pocoWrapper<FRAME_SIZE>(
          executionContext_.host(),
          executionContext_.port(),
          executionContext_.uri()
      );

      while (continueRunning) {
        std::span<char> result = delegate.receive(flags);
        SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, result.data(), static_cast<int>(result.size()));
        const std::variant<std::monostate, std::string> &parseResult = parser.parse(message);
        if (std::holds_alternative<std::monostate>(parseResult)) {
          return resultFn_(Failure{"WebSocket connection closed"});
        }

        std::cout << std::get<std::string>(parseResult) << std::endl;
      }
    } catch (const std::exception &e) {
       return resultFn_(Failure{e.what()});
    }

     return resultFn_(std::monostate());
  }

private:
  ExecutionContext executionContext_;
  std::function<A(const std::variant<Failure, std::monostate>&)> resultFn_;
};