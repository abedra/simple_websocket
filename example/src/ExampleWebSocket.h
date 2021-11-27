#pragma once

#include <utility>

#include "ExampleFrameParser.h"

struct ExampleWebSocket final {
  explicit ExampleWebSocket(std::string host,
                            Poco::UInt16 port,
                            std::string uri)
      : host_(std::move(host))
      , port_(port)
      , uri_(std::move(uri))
  { }

  std::variant<std::string, Poco::Net::WebSocket> buildWebSocket() {
    Poco::Net::HTTPClientSession session{host_, port_};
    Poco::Net::HTTPRequest request{Poco::Net::HTTPRequest::HTTP_GET, uri_, Poco::Net::HTTPMessage::HTTP_1_1};
    Poco::Net::HTTPResponse response;

    try {
      return Poco::Net::WebSocket{session, request, response};
    } catch (const std::exception &e) {
      return e.what();
    }
  }

  static std::variant<std::string, std::monostate> start(Poco::Net::WebSocket &webSocket,
                                                         int frameSize,
                                                         const bool &continueRunning) {
    int flags;
    char buf[frameSize];
    ExampleFrameParser testFrameParser;
    SimpleWebSocket::MessageParser<std::string> parser{std::make_unique<ExampleFrameParser>(testFrameParser)};

    try {
      do {
        int received = webSocket.receiveFrame(buf, frameSize, flags);
        SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, buf, received);
        std::string parsed = parser.parse(message);
        std::cout << parsed << std::endl;
        memset(buf, '\0', frameSize);
      } while (continueRunning);
    } catch (const std::exception &e) {
      return e.what();
    }

    return std::monostate();
  }

private:
  std::string host_;
  Poco::UInt16 port_;
  std::string uri_;
};