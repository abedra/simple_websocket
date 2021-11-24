#include <iostream>
#include <csignal>
#include <sysexits.h>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include <simple_websocket.hpp>
#include <csignal>
#include <utility>
#include <variant>
#include <sysexits.h>

template<class... As> struct match : As... { using As::operator()...; };
template<class... As> match(As...) -> match<As...>;

constexpr int FRAME_SIZE = 1024;
bool continueRunning = true;

void shutDown(int _) {
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
    throw std::runtime_error("ERROR: received undefined frame op code");
  }

  std::string handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    return textFrame.value();
  }
};

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

  static std::variant<std::string, std::monostate> start(Poco::Net::WebSocket &webSocket) {
    int flags;
    char buf[FRAME_SIZE];
    Poco::Net::WebSocket webSocket{session, request, response};
    TestFrameParser testFrameParser;
    SimpleWebSocket::MessageParser<std::string> parser{std::make_unique<TestFrameParser>(testFrameParser)};

    try {
      do {
        int received = webSocket.receiveFrame(buf, FRAME_SIZE, flags);
        SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, buf, received);
        std::string parsed = parser.parse(message);
        std::cout << parsed << std::endl;
        memset(buf, '\0', FRAME_SIZE);
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

int main() {
  signal(SIGHUP, shudDown);
  signal(SIGINT, shudDown);
  signal(SIGTERM, shudDown);

  std::variant<std::string, std::monostate> sentinel = "";
  do {
    ExampleWebSocket exampleWebSocket{"localhost", 8080, "/"};
    std::variant<std::string, Poco::Net::WebSocket> maybeWebSocket = exampleWebSocket.buildWebSocket();

    std::visit(match{
        [](const std::string &error) {
          std::cout << error << std::endl;
          sleep(1);
        },
        [&sentinel](Poco::Net::WebSocket &webSocket) {
          std::variant<std::string, std::monostate> result = ExampleWebSocket::start(webSocket);
          if (holds_alternative<std::string>(result)) {
            std::cout << get<std::string>(result) << std::endl;
            sleep(1);
          }

          sentinel = result;
        }
    }, maybeWebSocket);
  } while (holds_alternative<std::string>(sentinel));

  return EX_OK;
}
