#ifndef REQUEST_H
#define REQUEST_H

#include <memory>
#include <string>
#include <boost/tokenizer.hpp>

// Represents an HTTP Request.
//
// Usage:
// [1] From a raw request:
//   auto request = Request::Parse(raw_request);
//   
// [2] Building a request from scratch:
//  Request r;
//  r.AddRequestLine("GET", "/", "HTTP/1.1"); 
//  r.AddHeader("Connection", "keep-alive");
//  r.SetBody(...); 
//  return r.ToString();
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

  void AddRequestLine(std::string method, 
                        std::string uri, 
                        std::string protocol); 

  void AddHeader(const std::string& header_name,
                 const std::string& header_value);

  void SetBody(const std::string& body);

  std::string ToString();

 private:
  std::string raw_request_;
  std::string method_;
  std::string uri_;
  std::string version_;
  Headers headers_;
  std::string body_;

  std::string request_line_; 
};

#endif
