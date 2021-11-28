#pragma once

#include <utility>
#include <span>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

struct PocoWrapper final {
  explicit PocoWrapper(const Poco::Net::WebSocket& webSocket, int frameSize)
    : webSocket_(webSocket)
    , frameSize_(frameSize)
    , buffer_(std::make_unique<char>(frameSize))
  { }

  ~PocoWrapper() {
    webSocket_.close();
  }

  [[nodiscard]]
  std::span<char> receive(int &flags) {
    int bytesReceived = webSocket_.receiveFrame(buffer_.get(), frameSize_, flags);
    return {buffer_.get(), static_cast<size_t>(bytesReceived)};
  }
  
  [[nodiscard]]
  int frameSize() const {
    return frameSize_;
  }

private:
  Poco::Net::WebSocket webSocket_;
  int frameSize_;
  std::unique_ptr<char> buffer_;
};
