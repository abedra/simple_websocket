#pragma once

#include <atomic>
#include <utility>

#include <Poco/Types.h>

#include "Variant.h"
#include "ExecutionContext.h"
#include "PocoWrapper.h"

template<int FRAME_SIZE, class A>
struct WebSocketClient final {
  explicit WebSocketClient(ExecutionContext executionContext,
                           std::atomic<bool> &continueRunning,
                           std::function<A(const std::variant<std::string, std::monostate>&)> resultFn)
    : executionContext_(std::move(executionContext))
    , resultFn_(resultFn)
    , continueRunning_(continueRunning)
  { }

   A start() {
    ExampleFrameParser frameParser;
    SimpleWebSocket::MessageParser<std::variant<std::monostate, std::string>> parser{
      std::make_unique<ExampleFrameParser>(frameParser)
    };

    PocoWrapper<FRAME_SIZE> delegate = pocoWrapper<FRAME_SIZE>(
        executionContext_.host(),
        executionContext_.port(),
        executionContext_.uri()
    );

    int flags;

    try {
      while (continueRunning_) {
        std::span<char> result = delegate.receive(flags);
        SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, result.data(), static_cast<int>(result.size()));
        const std::variant<std::monostate, std::string> &parseResult = parser.parse(message);
        if (std::holds_alternative<std::monostate>(parseResult)) {
          return resultFn_("WebSocket connection closed");
        }

        std::cout << std::get<std::string>(parseResult) << std::endl;
      }
    } catch (const std::exception &e) {
       return resultFn_(e.what());
    }

     return resultFn_(std::monostate());
  }

private:
  ExecutionContext executionContext_;
  std::function<A(const std::variant<std::string, std::monostate>&)> resultFn_;
  std::atomic<bool> &continueRunning_;
};