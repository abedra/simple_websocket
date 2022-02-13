#include <iostream>
#include <csignal>
#include <sysexits.h>

#include "Types.h"
#include "WebSocketClient.h"

std::atomic<bool> continueRunning = true;

void shutDown([[maybe_unused]] int _) {
  std::cout << "Received shutdown signal, terminating" << std::endl;
  continueRunning = false;
}

int main() {
  signal(SIGHUP, shutDown);
  signal(SIGINT, shutDown);
  signal(SIGTERM, shutDown);
  signal(SIGKILL, shutDown);
  signal(SIGQUIT, shutDown);

  constexpr int FRAME_SIZE = 1024;

  WebSocketClient<FRAME_SIZE, WorkflowResult> webSocketClient{
    ExecutionContext{"localhost", 8080, "/"},
    [](const auto& result) { return WorkflowResult{result}; }
  };

  Workflow workflow{
    [&, client = std::move(webSocketClient)]() { return client.start(continueRunning); },
    [](const auto&) { },
    [](const Failure &failure) {
      std::cout << failure << std::endl;
      sleep(1);
    }
  };

  workflow.runUntilCancelled();

  return EX_OK;
}
