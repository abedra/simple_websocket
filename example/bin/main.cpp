#include <iostream>
#include <csignal>
#include <sysexits.h>

#include <simple_websocket.hpp>

#include "WebSocketWorkflow.h"

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

  ExampleWebSocket executionContext{"localhost", 8080, "/", FRAME_SIZE};
  WebSocketWorkflow workflow{executionContext, continueRunning};
  workflow.runUntilCancelled();

  return EX_OK;
}
