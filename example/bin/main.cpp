#include <iostream>
#include <csignal>
#include <sysexits.h>
#include <variant>

#include "Workflow.h"
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

  constexpr int FRAME_SIZE = 1024;
  ExecutionContext executionContext{"localhost", 8080, "/"};

  WebSocketClient<FRAME_SIZE, WorkflowResult<std::string>> webSocketClient{
    executionContext,
    continueRunning,
    [](const std::variant<std::string, std::monostate>& result) {
      return WorkflowResult{result};
    }
  };

  Workflow<std::string> workflow{
    [&webSocketClient](){ return webSocketClient.start(); },
    [](const std::monostate &_){ },
    [](const std::string &failure) {
      std::cout << failure << std::endl;
      sleep(1);
    }
  };

  workflow.runUntilCancelled();

  return EX_OK;
}
