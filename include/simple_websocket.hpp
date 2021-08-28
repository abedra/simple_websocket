#pragma once

#include <string>
#include <utility>
#include <vector>
#include <variant>
#include <memory>

namespace SimpleWebSocket {
  struct PingFrame final {
    explicit PingFrame(std::string value) : value_(std::move(value)) {}

    [[nodiscard]]
    const std::string &value() const {
      return value_;
    }

  private:
    std::string value_;
  };

  struct PongFrame final {
    explicit PongFrame(std::string value) : value_(std::move(value)) {}

    [[nodiscard]]
    const std::string &value() const {
      return value_;
    }

  private:
    std::string value_;
  };

  struct TextFrame final {
    explicit TextFrame(std::string value) : value_(std::move(value)) {}

    [[nodiscard]]
    const std::string &value() const {
      return value_;
    }

  private:
    std::string value_;
  };

  struct BinaryFrame final {
    explicit BinaryFrame(std::vector<char> value) : value_(std::move(value)) {}

    [[nodiscard]]
    const std::vector<char> &value() const {
      return value_;
    }

  private:
    std::vector<char> value_;
  };

  struct CloseFrame final {
    explicit CloseFrame(std::string value) : value_(std::move(value)) {}

    [[nodiscard]]
    const std::string &value() const {
      return value_;
    }

  private:
    std::string value_;
  };

  struct UndefinedFrame final { };

  struct FrameHandler {
    virtual void handlePing(const PingFrame& pingFrame) = 0;
    virtual void handlePong(const PongFrame& pongFrame) = 0;
    virtual void handleText(const TextFrame& textFrame) = 0;
    virtual void handleBinary(const BinaryFrame& binaryFrame) = 0;
    virtual void handleClose(const CloseFrame& closeFrame) = 0;
    virtual void handleUndefined(const UndefinedFrame& undefinedFrame) = 0;
  };

  struct Message final {
    explicit Message(std::variant<PingFrame, PongFrame, TextFrame, BinaryFrame, CloseFrame, UndefinedFrame> value)
      : value_(std::move(value))
    { }

    [[nodiscard]]
    const std::variant<PingFrame, PongFrame, TextFrame, BinaryFrame, CloseFrame, UndefinedFrame> &value() const {
      return value_;
    }

  private:
    std::variant<PingFrame, PongFrame, TextFrame, BinaryFrame, CloseFrame, UndefinedFrame> value_;
  };

  template<class... As> struct match : As... { using As::operator()...; };
  template<class... As> match(As...) -> match<As...>;
  struct MessageHandler final {
    explicit MessageHandler(std::unique_ptr<FrameHandler> delegate) : delegate_(std::move(delegate)) {}

    void handle(const Message& message) {
      std::visit(match{
        [this](const PingFrame& pingFrame) { delegate_->handlePing(pingFrame); },
        [this](const PongFrame& pongFrame) { delegate_->handlePong(pongFrame); },
        [this](const TextFrame& textFrame) { delegate_->handleText(textFrame); },
        [this](const BinaryFrame& binaryFrame) { delegate_->handleBinary(binaryFrame); },
        [this](const CloseFrame& closeFrame) { delegate_->handleClose(closeFrame); },
        [this](const UndefinedFrame& undefinedFrame) { delegate_->handleUndefined(undefinedFrame); },
      }, message.value());
    }

  private:
    std::unique_ptr<FrameHandler> delegate_;
  };
}