#include <iostream>
#include <csignal>
#include <sysexits.h>

#include <simple_websocket.hpp>

#include "Workflow.h"

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

  Workflow<std::string, std::monostate, ExampleWebSocket> workflow{continueRunning};
  ExampleWebSocket webSocket{"localhost", 8080, "/", FRAME_SIZE};

  workflow.runUntil(
      [](const Either<std::string, std::monostate> &result) { return result.isRight(); },
      [](const std::string &failure) {
        std::cout << failure << std::endl;
        sleep(1);
      },
      webSocket
  );

  return EX_OK;
}
