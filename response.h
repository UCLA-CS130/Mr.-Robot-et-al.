#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

// Represents an HTTP response.
//
// Usage:
//   Response r;
//   r.SetStatus(RESPONSE_200);
//   r.SetBody(...);
//   return r.ToString();
//
// Constructed by the RequestHandler, 
// after which the server should call ToString
// to serialize.
class Response {
 public:
  enum ResponseCode {
    OK = 200,
    NOT_FOUND = 404
  };
  
  void SetStatus(const ResponseCode response_code);
  void AddHeader(const std::string& header_name, 
                 const std::string& header_value);
  void SetBody(const std::string& body);
  
  std::string ToString();

 private:
  std::string response_header_;
  std::string response_body_;
};

#endif
