#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <utility>
#include "simple_websocket.hpp"

struct TestFrameHandler final : SimpleWebSocket::FrameHandler {
  explicit TestFrameHandler(std::vector<std::string>& messages) : messages_(messages) {}

  void handlePing(const SimpleWebSocket::PingFrame &pingFrame) override {
    messages_.emplace_back(pingFrame.value());
  }

  void handlePong(const SimpleWebSocket::PongFrame &pongFrame) override {
    messages_.emplace_back(pongFrame.value());
  }

  void handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    messages_.emplace_back(textFrame.value());
  }

  void handleBinary(const SimpleWebSocket::BinaryFrame &binaryFrame) override {
    std::stringstream ss;
    for (const auto& c : binaryFrame.value()) {
      ss << c;
    }
    messages_.emplace_back(ss.str());
  }

  void handleClose(const SimpleWebSocket::CloseFrame &closeFrame) override {
    messages_.emplace_back(closeFrame.value());
  }

  void handleUndefined(const SimpleWebSocket::UndefinedFrame &undefinedFrame) override {
    messages_.emplace_back("UNDEFINED");
  }

private:
  std::vector<std::string>& messages_;
};

TEST_CASE("PingFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::PingFrame{"ping"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "ping");
}

TEST_CASE("PongFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::PongFrame{"pong"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "pong");
}

TEST_CASE("TextFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::TextFrame{"text"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "text");
}

TEST_CASE("BinaryFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::BinaryFrame{{'b', 'i', 'n', 'a', 'r', 'y'}}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "binary");
}

TEST_CASE("CloseFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::CloseFrame{"close"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "close");
}

TEST_CASE("UndefinedFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::UndefinedFrame{}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "UNDEFINED");
}

TEST_CASE("Mixed Frames")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message ping{SimpleWebSocket::PingFrame{"ping"}};
  SimpleWebSocket::Message text{SimpleWebSocket::TextFrame{"text"}};
  SimpleWebSocket::Message close{SimpleWebSocket::CloseFrame{"close"}};
  messageHandler.handle(ping);
  messageHandler.handle(text);
  messageHandler.handle(close);

  REQUIRE(messages.size() == 3);
  CHECK(messages.at(0) == "ping");
  CHECK(messages.at(1) == "text");
  CHECK(messages.at(2) == "close");
}

