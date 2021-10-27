#include <iostream>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <simple_websocket.hpp>
#include <csignal>

constexpr int FRAME_SIZE = 1024;

bool continueRunning = true;

void handleSignal(int _) {
  std::cout << "Received shutdown signal, terminating" << std::endl;
  continueRunning = false;
}

struct TestFrameParser final : SimpleWebSocket::FrameParser<std::string> {
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
    return "ERROR: received undefined frame op code";
  }

  std::string handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    return textFrame.value();
  }
};

int main() {
  signal(SIGHUP, handleSignal);
  signal(SIGINT, handleSignal);
  signal(SIGTERM, handleSignal);

  Poco::Net::HTTPClientSession session{"localhost", 8080};
  Poco::Net::HTTPRequest request{Poco::Net::HTTPRequest::HTTP_GET, "/", Poco::Net::HTTPMessage::HTTP_1_1};
  Poco::Net::HTTPResponse response;

  try {
    int flags;
    char buf[FRAME_SIZE];
    Poco::Net::WebSocket webSocket{session, request, response};
    TestFrameParser testFrameParser;
    SimpleWebSocket::MessageParser<std::string> parser{std::make_unique<TestFrameParser>(testFrameParser)};

    do {
      int received = webSocket.receiveFrame(buf, FRAME_SIZE, flags);
      SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, buf, received);
      std::string parsed = parser.parse(message);
      std::cout << parsed << std::endl;
      memset(buf, '\0', FRAME_SIZE);
    } while (continueRunning);
  } catch (const std::exception &e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
