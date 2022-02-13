#pragma once

#include <span>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/WebSocket.h>

template<int SIZE>
struct PocoWrapper final {
  explicit PocoWrapper(const Poco::Net::WebSocket& webSocket)
    : webSocket_(webSocket)
  { }

  ~PocoWrapper() {
    webSocket_.close();
  }

  [[nodiscard]]
  std::span<char> receive(int &flags) {
    char buffer[SIZE];
    int bytesReceived = webSocket_.receiveFrame(buffer, SIZE, flags);
    return {buffer, static_cast<size_t>(bytesReceived)};
  }

private:
  Poco::Net::WebSocket webSocket_;
};

template<int SIZE>
inline PocoWrapper<SIZE> pocoWrapper(const std::string& host,
                                     Poco::UInt16 port,
                                     const std::string& uri) {
  Poco::Net::HTTPClientSession session{host, port};
  Poco::Net::HTTPRequest request{Poco::Net::HTTPRequest::HTTP_GET, uri, Poco::Net::HTTPMessage::HTTP_1_1};
  Poco::Net::HTTPResponse response;

  return PocoWrapper<SIZE>{Poco::Net::WebSocket{session, request, response}};
}