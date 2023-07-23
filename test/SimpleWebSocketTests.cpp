#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../simple_websocket.hpp"

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

struct TestFrameParser final : SimpleWebSocket::FrameParser<std::string> {
  std::string handlePing(const SimpleWebSocket::PingFrame &pingFrame) override {
    return pingFrame.value();
  }

  std::string handlePong(const SimpleWebSocket::PongFrame &pongFrame) override {
    return pongFrame.value();
  }

  std::string handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    return textFrame.value();
  }

  std::string handleBinary(const SimpleWebSocket::BinaryFrame &binaryFrame) override {
    std::stringstream ss;
    for (const auto& c : binaryFrame.value()) {
      ss << c;
    }
    return ss.str();
  }

  std::string handleClose(const SimpleWebSocket::CloseFrame &closeFrame) override {
    return closeFrame.value();
  }

  std::string handleUndefined(const SimpleWebSocket::UndefinedFrame &undefinedFrame) override {
    return "";
  }
};

TEST_CASE("Handle PingFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::PingFrame{"ping"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "ping");
}

TEST_CASE("Parse PingFrame")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message message{SimpleWebSocket::PingFrame{"ping"}};

  CHECK("ping" == messageParser.parse(message));
}

TEST_CASE("Handle PongFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::PongFrame{"pong"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "pong");
}

TEST_CASE("Parse PongFrame")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message message{SimpleWebSocket::PongFrame{"pong"}};

  CHECK("pong" == messageParser.parse(message));
}

TEST_CASE("Handle TextFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::TextFrame{"text"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "text");
}

TEST_CASE("Parse TextFrame")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message message{SimpleWebSocket::TextFrame{"text"}};

  CHECK("text" == messageParser.parse(message));
}

TEST_CASE("Handle BinaryFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::BinaryFrame{{'b', 'i', 'n', 'a', 'r', 'y'}}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "binary");
}

TEST_CASE("Parse BinaryFrame")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message message{SimpleWebSocket::BinaryFrame{{'b', 'i', 'n', 'a', 'r', 'y'}}};

  CHECK("binary" == messageParser.parse(message));
}

TEST_CASE("Handle CloseFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::CloseFrame{"close"}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "close");
}

TEST_CASE("Parse CloseFrame")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message message{SimpleWebSocket::CloseFrame{"close"}};

  CHECK("close" == messageParser.parse(message));
}

TEST_CASE("Handle UndefinedFrame")
{
  std::vector<std::string> messages;
  SimpleWebSocket::MessageHandler messageHandler{std::make_unique<TestFrameHandler>(messages)};
  SimpleWebSocket::Message message{SimpleWebSocket::UndefinedFrame{}};
  messageHandler.handle(message);

  REQUIRE(messages.size() == 1);
  CHECK(messages.at(0) == "UNDEFINED");
}

TEST_CASE("Parse UndefinedFrame")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message message{SimpleWebSocket::UndefinedFrame{}};

  CHECK(messageParser.parse(message).empty());
}

TEST_CASE("Handle Mixed Frames")
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

TEST_CASE("Parse MixedFrames")
{
  SimpleWebSocket::MessageParser<std::string> messageParser{std::make_unique<TestFrameParser>()};
  SimpleWebSocket::Message ping{SimpleWebSocket::PingFrame{"ping"}};
  SimpleWebSocket::Message text{SimpleWebSocket::TextFrame{"text"}};
  SimpleWebSocket::Message close{SimpleWebSocket::CloseFrame{"close"}};

  CHECK("ping" == messageParser.parse(ping));
  CHECK("text" == messageParser.parse(text));
  CHECK("close" == messageParser.parse(close));
}

TEST_CASE("Poco PING_FRAME")
{
  int flags = 137;
  char buf[5] = "ping";
  int size = 4;

  SimpleWebSocket::Message actual = SimpleWebSocket::Poco::fromPoco(flags, buf, size);
  SimpleWebSocket::Message expected = SimpleWebSocket::Message{SimpleWebSocket::PingFrame{"ping"}};
  CHECK(expected == actual);
}

TEST_CASE("Poco PONG_FRAME")
{
  int flags = 138;
  char buf[5] = "pong";
  int size = 4;

  SimpleWebSocket::Message actual = SimpleWebSocket::Poco::fromPoco(flags, buf, size);
  SimpleWebSocket::Message expected = SimpleWebSocket::Message{SimpleWebSocket::PongFrame{"pong"}};
  CHECK(expected == actual);
}

TEST_CASE("Poco TEXT_FRAME")
{
  int flags = 129;
  char buf[5] = "text";
  int size = 4;

  SimpleWebSocket::Message actual = SimpleWebSocket::Poco::fromPoco(flags, buf, size);
  SimpleWebSocket::Message expected = SimpleWebSocket::Message{SimpleWebSocket::TextFrame{"text"}};
  CHECK(expected == actual);
}

TEST_CASE("Poco BINARY_FRAME")
{
  int flags = 130;
  char buf[7] = "binary";
  int size = 6;

  SimpleWebSocket::Message actual = SimpleWebSocket::Poco::fromPoco(flags, buf, size);
  SimpleWebSocket::Message expected = SimpleWebSocket::Message{SimpleWebSocket::BinaryFrame{{'b', 'i', 'n', 'a', 'r', 'y'}}};
  CHECK(expected == actual);
}

TEST_CASE("Poco CLOSE_FRAME")
{
  int flags = 136;
  char buf[6] = "close";
  int size = 5;

  SimpleWebSocket::Message actual = SimpleWebSocket::Poco::fromPoco(flags, buf, size);
  SimpleWebSocket::Message expected = SimpleWebSocket::Message{SimpleWebSocket::CloseFrame{"close"}};
  CHECK(expected == actual);
}

TEST_CASE("Poco Undefined")
{
  int flags = 0;
  char buf[1] = "";
  int size = 1;

  SimpleWebSocket::Message actual = SimpleWebSocket::Poco::fromPoco(flags, buf, size);
  SimpleWebSocket::Message expected = SimpleWebSocket::Message{SimpleWebSocket::UndefinedFrame{}};
  CHECK(expected == actual);
}