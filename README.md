# Simple WebSocket

A small header only C++ library that provides the foundation of an algebraic data type for parsing WebSocket frames.

## Usage

The following examples attempt to demonstrate the two shapes offered by this library. For a complete working example using the Poco framework, take a look at the [example](https://github.com/abedra/simple_websocket/tree/master/example).

### Frame Parser

If you want to keep the parsing of frames pure, you can use the `SimpleWebSocket::FrameParser` template class. This template allows you to specify a unifying type for all frames, and expects you to provide the arrows from each of the message variant members to your chosen type. A contrived, yet simple example:

```c++
struct TestFrameParser final : SimpleWebSocket::FrameParser<std::string> {
  std::string handlePing(const SimpleWebSocket::PingFrame &pingFrame) override {
    return pingFrame.value();
  }

  std::string handlePong(const SimpleWebSocket::PongFrame &pongFrame) override {
    return pongFrame.value();
  }

  std::string handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    return textFrame.value();
  }

  std::string handleBinary(const SimpleWebSocket::BinaryFrame &binaryFrame) override {
    std::stringstream ss;
    for (const auto& c : binaryFrame.value()) {
      ss << c;
    }
    return ss.str();
  }

  std::string handleClose(const SimpleWebSocket::CloseFrame &closeFrame) override {
    return closeFrame.value();
  }

  std::string handleUndefined(const SimpleWebSocket::UndefinedFrame &undefinedFrame) override {
    return "";
  }
};
```

Under real world circumstances, you would likely provide yet another `std::variant` as your `A` if you expect to handle either multiple frame types or multiple payload types.

### Frame Handler

If your frame processing has side effects, or if you simply want to push the results of frame processing somewhere else as quickly as possible, you can use the `SimpleWebSocket::FrameHandler` class. This class does not demand any return types from its constituent handlers, so all processing results must perform some kind of side effect to push results outside the handler. For example:

```c++
struct TestFrameHandler final : SimpleWebSocket::FrameHandler {
  explicit TestFrameHandler(std::vector<std::string>& messages) : messages_(messages) {}

  void handlePing(const SimpleWebSocket::PingFrame &pingFrame) override {
    messages_.emplace_back(pingFrame.value());
  }

  void handlePong(const SimpleWebSocket::PongFrame &pongFrame) override {
    messages_.emplace_back(pongFrame.value());
  }

  void handleText(const SimpleWebSocket::TextFrame &textFrame) override {
    messages_.emplace_back(textFrame.value());
  }

  void handleBinary(const SimpleWebSocket::BinaryFrame &binaryFrame) override {
    std::stringstream ss;
    for (const auto& c : binaryFrame.value()) {
      ss << c;
    }
    messages_.emplace_back(ss.str());
  }

  void handleClose(const SimpleWebSocket::CloseFrame &closeFrame) override {
    messages_.emplace_back(closeFrame.value());
  }

  void handleUndefined(const SimpleWebSocket::UndefinedFrame &undefinedFrame) override {
    messages_.emplace_back("UNDEFINED");
  }

private:
  std::vector<std::string>& messages_;
};
```

## WebSocket Library Helpers

### Poco

Given a buffer, a message length, and the flags populated on receive, the `SimpleWebSocket::fromPoco` method will properly extract the buffer into a `SimpleWebSocket::Message` type. This makes it easy to then pass the data into a frame handler or frame parser for further processing.

```c++
int received = webSocket.receiveFrame(buf, FRAME_SIZE, flags);
SimpleWebSocket::Message message = SimpleWebSocket::Poco::fromPoco(flags, buf, received);
```

### Boost Beast (TODO)