#ifndef REQUEST_H
#define REQUEST_H

#include <memory>
#include <string>
#include <boost/tokenizer.hpp>

// Represents an HTTP Request.
//
// Usage:
//   auto request = Request::Parse(raw_request);
class Request {
 public:
  // raw_request is the string read from socket
  static std::unique_ptr<Request> Parse(const std::string& raw_request);

  std::string raw_request() const;
  std::string method() const;
  std::string uri() const;
  std::string version() const;

  using Headers = std::vector<std::pair<std::string, std::string>>;
  Headers headers() const;

  std::string body() const;

 private:
  std::string raw_request_;
  std::string method_;
  std::string uri_;
  std::string version_;
  Headers headers_;
  std::string body_;
};

#endif
