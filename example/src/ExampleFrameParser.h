#pragma once

struct ExampleFrameParser final : SimpleWebSocket::FrameParser<std::string> {
  std::string handlePing(const SimpleWebSocket::PingFrame &pingFrame) override {
    return "ERROR: received unexpected ping frame";
  }

  std::string handlePong(const SimpleWebSocket::PongFrame &pongFrame) override {
    return "ERROR: received unexpected pong frame";
  }

  std::string handleBinary(const SimpleWebSocket::BinaryFrame &binaryFrame) override {
    return "ERROR: received unexpected binary frame";
  }

  std::string handleClose(const SimpleWebSocket::CloseFrame &closeFrame) override {
    return "ERROR: received unexpected close frame";
  }

  std::string handleUndefined(const SimpleWebSocket::UndefinedFrame &undefinedFrame) override {
    throw std::runtime_error("ERROR: received undefined frame op code");
  }

  std::string handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    return textFrame.value();
  }
};
