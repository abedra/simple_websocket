#pragma once

#include <string>
#include <utility>
#include <vector>
#include <variant>
#include <memory>

namespace SimpleWebSocket {
    template<class... As>
    struct visitor : As ... {
        using As::operator()...;
    };
    template<class... As> visitor(As...) -> visitor<As...>;

    struct Failure final {
        explicit Failure(std::string value) : value_(std::move(value)) {}

        friend std::ostream &operator<<(std::ostream &os, const Failure &failure) {
            os << failure.value_;
            return os;
        }

        [[nodiscard]] const std::string &value() const {
            return value_;
        }

    private:
        std::string value_;
    };


    struct PingFrame final {
        explicit PingFrame(std::string value) : value_(std::move(value)) {}

        bool operator==(const PingFrame &rhs) const {
            return value_ == rhs.value_;
        }

        bool operator!=(const PingFrame &rhs) const {
            return !(rhs == *this);
        }

        [[nodiscard]]
        const std::string &value() const {
            return value_;
        }

    private:
        std::string value_;
    };

    struct PongFrame final {
        explicit PongFrame(std::string value) : value_(std::move(value)) {}

        bool operator==(const PongFrame &rhs) const {
            return value_ == rhs.value_;
        }

        bool operator!=(const PongFrame &rhs) const {
            return !(rhs == *this);
        }

        [[nodiscard]]
        const std::string &value() const {
            return value_;
        }

    private:
        std::string value_;
    };

    struct TextFrame final {
        explicit TextFrame(std::string value) : value_(std::move(value)) {}

        bool operator==(const TextFrame &rhs) const {
            return value_ == rhs.value_;
        }

        bool operator!=(const TextFrame &rhs) const {
            return !(rhs == *this);
        }

        [[nodiscard]]
        const std::string &value() const {
            return value_;
        }

    private:
        std::string value_;
    };

    struct BinaryFrame final {
        explicit BinaryFrame(std::vector<char> value) : value_(std::move(value)) {}

        bool operator==(const BinaryFrame &rhs) const {
            return value_ == rhs.value_;
        }

        bool operator!=(const BinaryFrame &rhs) const {
            return !(rhs == *this);
        }

        [[nodiscard]]
        const std::vector<char> &value() const {
            return value_;
        }

    private:
        std::vector<char> value_;
    };

    struct CloseFrame final {
        explicit CloseFrame(std::string value) : value_(std::move(value)) {}

        bool operator==(const CloseFrame &rhs) const {
            return value_ == rhs.value_;
        }

        bool operator!=(const CloseFrame &rhs) const {
            return !(rhs == *this);
        }

        [[nodiscard]]
        const std::string &value() const {
            return value_;
        }

    private:
        std::string value_;
    };

    struct UndefinedFrame final {
        bool operator==(const UndefinedFrame &_) const {
            return true;
        }

        bool operator!=(const UndefinedFrame &_) const {
            return false;
        }
    };

    struct FrameHandler {
        virtual ~FrameHandler() = default;

        virtual void handlePing(const PingFrame &pingFrame) = 0;

        virtual void handlePong(const PongFrame &pongFrame) = 0;

        virtual void handleText(const TextFrame &textFrame) = 0;

        virtual void handleBinary(const BinaryFrame &binaryFrame) = 0;

        virtual void handleClose(const CloseFrame &closeFrame) = 0;

        virtual void handleUndefined(const UndefinedFrame &undefinedFrame) = 0;
    };

    template<class A>
    struct FrameParser {
        virtual ~FrameParser() = default;

        virtual A handlePing(const PingFrame &pingFrame) = 0;

        virtual A handlePong(const PongFrame &pongFrame) = 0;

        virtual A handleText(const TextFrame &textFrame) = 0;

        virtual A handleBinary(const BinaryFrame &binaryFrame) = 0;

        virtual A handleClose(const CloseFrame &closeFrame) = 0;

        virtual A handleUndefined(const UndefinedFrame &undefinedFrame) = 0;
    };

    struct Message final {
        explicit Message(std::variant<PingFrame, PongFrame, TextFrame, BinaryFrame, CloseFrame, UndefinedFrame> value)
                : value_(std::move(value)) {}

        bool operator==(const Message &rhs) const {
            if (std::holds_alternative<PingFrame>(value_) && std::holds_alternative<PingFrame>(rhs.value())) {
                return std::get<PingFrame>(value_) == std::get<PingFrame>(rhs.value());
            }

            if (std::holds_alternative<PongFrame>(value_) && std::holds_alternative<PongFrame>(rhs.value())) {
                return std::get<PongFrame>(value_) == std::get<PongFrame>(rhs.value());
            }

            if (std::holds_alternative<TextFrame>(value_) && std::holds_alternative<TextFrame>(rhs.value())) {
                return std::get<TextFrame>(value_) == std::get<TextFrame>(rhs.value());
            }

            if (std::holds_alternative<BinaryFrame>(value_) && std::holds_alternative<BinaryFrame>(rhs.value())) {
                return std::get<BinaryFrame>(value_) == std::get<BinaryFrame>(rhs.value());
            }

            if (std::holds_alternative<CloseFrame>(value_) && std::holds_alternative<CloseFrame>(rhs.value())) {
                return std::get<CloseFrame>(value_) == std::get<CloseFrame>(rhs.value());
            }

            if (std::holds_alternative<UndefinedFrame>(value_) && std::holds_alternative<UndefinedFrame>(rhs.value())) {
                return std::get<UndefinedFrame>(value_) == std::get<UndefinedFrame>(rhs.value());
            }

            return false;
        }

        bool operator!=(const Message &rhs) const {
            return !(rhs == *this);
        }

        [[nodiscard]]
        const std::variant<PingFrame, PongFrame, TextFrame, BinaryFrame, CloseFrame, UndefinedFrame> &value() const {
            return value_;
        }

    private:
        std::variant<PingFrame, PongFrame, TextFrame, BinaryFrame, CloseFrame, UndefinedFrame> value_;
    };

    struct MessageHandler final {
        explicit MessageHandler(std::unique_ptr<FrameHandler> delegate) : delegate_(std::move(delegate)) {}

        virtual ~MessageHandler() = default;

        void handle(const Message &message) {
            std::visit(visitor{
                    [this](const PingFrame &pingFrame) { delegate_->handlePing(pingFrame); },
                    [this](const PongFrame &pongFrame) { delegate_->handlePong(pongFrame); },
                    [this](const TextFrame &textFrame) { delegate_->handleText(textFrame); },
                    [this](const BinaryFrame &binaryFrame) { delegate_->handleBinary(binaryFrame); },
                    [this](const CloseFrame &closeFrame) { delegate_->handleClose(closeFrame); },
                    [this](const UndefinedFrame &undefinedFrame) { delegate_->handleUndefined(undefinedFrame); },
            }, message.value());
        }

    private:
        std::unique_ptr<FrameHandler> delegate_;
    };

    template<class A>
    struct MessageParser final {
        explicit MessageParser(std::unique_ptr<FrameParser<A>> delegate) : delegate_(std::move(delegate)) {}

        virtual ~MessageParser() = default;

        A parse(const Message &message) {
            return std::visit(visitor{
                    [this](const PingFrame &pingFrame) { return delegate_->handlePing(pingFrame); },
                    [this](const PongFrame &pongFrame) { return delegate_->handlePong(pongFrame); },
                    [this](const TextFrame &textFrame) { return delegate_->handleText(textFrame); },
                    [this](const BinaryFrame &binaryFrame) { return delegate_->handleBinary(binaryFrame); },
                    [this](const CloseFrame &closeFrame) { return delegate_->handleClose(closeFrame); },
                    [this](const UndefinedFrame &undefinedFrame) { return delegate_->handleUndefined(undefinedFrame); },
            }, message.value());
        }

    private:
        std::unique_ptr<FrameParser<A>> delegate_;
    };

    struct WorkflowResult final {
        explicit WorkflowResult(const std::monostate &unit) : value_(unit) {}

        explicit WorkflowResult(const Failure &f) : value_(f) {}

        explicit WorkflowResult(std::variant<Failure, std::monostate> value) : value_(std::move(value)) {}

        [[nodiscard]] const std::variant<Failure, std::monostate> &value() const {
            return value_;
        }

        [[nodiscard]] bool complete() const {
            return std::holds_alternative<std::monostate>(value_);
        }

        template<class R>
        [[nodiscard]] R
        match(const std::function<R(const Failure &a)> aFn, const std::function<R(const std::monostate &unit)> uFn) {
            return std::visit(visitor{
                    [&aFn](const Failure &a) { return aFn(a); },
                    [&uFn](const std::monostate &u) { return uFn(u); }
            }, value_);
        }

    private:
        std::variant<Failure, std::monostate> value_;
    };

    struct ExecutionContext final {
        ExecutionContext(std::string host, uint16_t port, std::string uri)
                : host_(std::move(host)), port_(port), uri_(std::move(uri)) {}

        [[nodiscard]] const std::string &host() const {
            return host_;
        }

        [[nodiscard]] uint16_t port() const {
            return port_;
        }

        [[nodiscard]] const std::string &uri() const {
            return uri_;
        }

    private:
        std::string host_;
        uint16_t port_;
        std::string uri_;
    };
}

#if __has_include(<Poco/Net/WebSocket.h>)
#include <span>
#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/WebSocket.h>

namespace SimpleWebSocket::Poco {
    constexpr int PING_FRAME   = static_cast<int>(::Poco::Net::WebSocket::FRAME_FLAG_FIN) |
                                 static_cast<int>(::Poco::Net::WebSocket::FRAME_OP_PING);
    constexpr int PONG_FRAME   = static_cast<int>(::Poco::Net::WebSocket::FRAME_FLAG_FIN) |
                                 static_cast<int>(::Poco::Net::WebSocket::FRAME_OP_PONG);
    constexpr int TEXT_FRAME   = static_cast<int>(::Poco::Net::WebSocket::FRAME_FLAG_FIN) |
                                 static_cast<int>(::Poco::Net::WebSocket::FRAME_OP_TEXT);
    constexpr int BINARY_FRAME = static_cast<int>(::Poco::Net::WebSocket::FRAME_FLAG_FIN) |
                                 static_cast<int>(::Poco::Net::WebSocket::FRAME_OP_BINARY);
    constexpr int CLOSE_FRAME  = static_cast<int>(::Poco::Net::WebSocket::FRAME_FLAG_FIN) |
                                 static_cast<int>(::Poco::Net::WebSocket::FRAME_OP_CLOSE);

    inline SimpleWebSocket::Message fromPoco(int flags, const char *buf, int size) {
      switch(flags) {
        case PING_FRAME:
          return SimpleWebSocket::Message{SimpleWebSocket::PingFrame{std::string(buf, size)}};
        case PONG_FRAME:
          return SimpleWebSocket::Message{SimpleWebSocket::PongFrame{std::string(buf, size)}};
        case TEXT_FRAME:
          return SimpleWebSocket::Message{SimpleWebSocket::TextFrame{std::string(buf, size)}};
        case BINARY_FRAME:
          return SimpleWebSocket::Message{SimpleWebSocket::BinaryFrame{std::vector<char>(buf, buf + size)}};
        case CLOSE_FRAME:
          return SimpleWebSocket::Message{SimpleWebSocket::CloseFrame{std::string(buf, size)}};
        default:
          return SimpleWebSocket::Message{SimpleWebSocket::UndefinedFrame{}};
      }
    }

    template<int SIZE>
    struct Wrapper final {
      explicit Wrapper(const ::Poco::Net::WebSocket& webSocket) : webSocket_(webSocket) { }

      ~Wrapper() {
        webSocket_.close();
      }

      [[nodiscard]] std::span<char> receive(int &flags) {
        char buffer[SIZE];
        int bytesReceived = webSocket_.receiveFrame(buffer, SIZE, flags);
        return {buffer, static_cast<size_t>(bytesReceived)};
      }
      
      int send(const std::string &message, int opCode) {
        return webSocket_.sendFrame(message.c_str(), static_cast<int>(message.length()), opCode);
      }
      
      int send(std::span<char> buffer, int opCode) {
        return webSocket_.sendBytes(buffer.data(), static_cast<int>(buffer.size()), opCode);
      }

    private:
      ::Poco::Net::WebSocket webSocket_;
    };

    template<int SIZE>
    inline Wrapper<SIZE> wrapper(const std::string& host,
                                 ::Poco::UInt16 port,
                                 const std::string& uri) {
      ::Poco::Net::HTTPClientSession session{host, port};
      ::Poco::Net::HTTPRequest request{::Poco::Net::HTTPRequest::HTTP_GET, uri, ::Poco::Net::HTTPMessage::HTTP_1_1};
      ::Poco::Net::HTTPResponse response;

      return Wrapper<SIZE>{::Poco::Net::WebSocket{session, request, response}};
    }

#if __has_include(<Poco/Net/SSLManager.h>)
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/HTTPSClientSession.h>

    template<int SIZE>
    inline Wrapper<SIZE> tls_wrapper(const std::string& host,
                                     ::Poco::UInt16 port,
                                     const std::string& uri) {
      ::Poco::Net::initializeSSL();
      ::Poco::Net::HTTPSClientSession session{host, port};
      ::Poco::Net::HTTPRequest request{::Poco::Net::HTTPRequest::HTTP_GET, uri, ::Poco::Net::HTTPMessage::HTTP_1_1};
      ::Poco::Net::HTTPResponse response;

      return Wrapper<SIZE>{::Poco::Net::WebSocket{session, request, response}};
    }
#endif
}
#endif