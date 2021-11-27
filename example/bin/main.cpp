#include <iostream>
#include <csignal>
#include <sysexits.h>
#include <variant>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <simple_websocket.hpp>

#include "ExampleWebSocket.h"

template<class... As> struct match : As... { using As::operator()...; };
template<class... As> match(As...) -> match<As...>;

constexpr int FRAME_SIZE = 1024;
bool continueRunning = true;

void shutDown(int _) {
  std::cout << "Received shutdown signal, terminating" << std::endl;
  continueRunning = false;
}

int main() {
  signal(SIGHUP, shutDown);
  signal(SIGINT, shutDown);
  signal(SIGTERM, shutDown);

  std::variant<std::string, std::monostate> workflowResult;

  do {
    ExampleWebSocket exampleWebSocket{"localhost", 8080, "/"};
    std::variant<std::string, Poco::Net::WebSocket> maybeWebSocket = exampleWebSocket.buildWebSocket();
    auto executionResult = std::visit(match{
        [](const std::string &error) {
          return std::variant<std::string, std::monostate>(error);
        },
        [](Poco::Net::WebSocket &webSocket) {
          return ExampleWebSocket::start(webSocket, FRAME_SIZE, continueRunning);
        }
    }, maybeWebSocket);

    if (holds_alternative<std::string>(executionResult)) {
      std::cout << get<std::string>(executionResult) << std::endl;
      sleep(1);
    }

    workflowResult = executionResult;
  } while (holds_alternative<std::string>(workflowResult));

  return EX_OK;
}
