#pragma once

struct ExecutionContext final {
  ExecutionContext(std::string host, unsigned short port, std::string uri)
      : host_(std::move(host))
      , port_(port)
      , uri_(std::move(uri))
  { }

  [[nodiscard]] const std::string &host() const {
    return host_;
  }

  [[nodiscard]] Poco::UInt16 port() const {
    return port_;
  }

  [[nodiscard]] const std::string &uri() const {
    return uri_;
  }

private:
  std::string host_;
  Poco::UInt16 port_;
  std::string uri_;
};
