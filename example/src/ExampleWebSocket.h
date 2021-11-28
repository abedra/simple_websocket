#pragma once

#include <utility>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include "PocoWrapper.h"

struct ExampleWebSocket final {
  explicit ExampleWebSocket(std::string host, Poco::UInt16 port, std::string uri, int frameSize)
    : host_(std::move(host))
    , port_(port)
    , uri_(std::move(uri))
    , frameSize_(frameSize)
  { }

  [[nodiscard]]
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

  [[nodiscard]]
  int frameSize() const {
    return frameSize_;
  }

private:
  std::string host_;
  Poco::UInt16 port_;
  std::string uri_;
  int frameSize_;
};