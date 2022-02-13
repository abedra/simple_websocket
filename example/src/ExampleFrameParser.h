#pragma once

#include <variant>
#include <simple_websocket.hpp>

struct ExampleFrameParser final : SimpleWebSocket::FrameParser<std::variant<std::monostate, std::string>> {
  std::variant<std::monostate, std::string> handlePing(const SimpleWebSocket::PingFrame &pingFrame) override {
    return "ERROR: received unexpected ping frame";
  }

  std::variant<std::monostate, std::string> handlePong(const SimpleWebSocket::PongFrame &pongFrame) override {
    return "ERROR: received unexpected pong frame";
  }

  std::variant<std::monostate, std::string> handleBinary(const SimpleWebSocket::BinaryFrame &binaryFrame) override {
    return "ERROR: received unexpected binary frame";
  }

  std::variant<std::monostate, std::string> handleClose(const SimpleWebSocket::CloseFrame &closeFrame) override {
    return std::monostate();
  }

  std::variant<std::monostate, std::string> handleUndefined(const SimpleWebSocket::UndefinedFrame &undefinedFrame) override {
    throw std::runtime_error("ERROR: received undefined frame op code");
  }

  std::variant<std::monostate, std::string> handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    return textFrame.value();
  }
};
