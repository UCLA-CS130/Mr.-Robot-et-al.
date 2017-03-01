#ifndef RESPONSE_H
#define RESPONSE_H

#include <memory>
#include <string>
#include <boost/tokenizer.hpp>
#include <map>

// Represents an HTTP response.
//
// Usage:
// [1] Building a response from scratch
//   Response r;
//   r.SetStatus(RESPONSE_200);
//   r.SetBody(...);
//   return r.ToString();
// [2] From a raw response
//   auto response = Response::Parse(raw_response);
//
// Constructed by the RequestHandler,
// after which the server should call ToString
// to serialize.
class Response {
 public:
    // If adding new response codes, update GetResponseCode
    enum ResponseCode {
    OK = 200,
    NOT_FOUND = 404,
    FOUND = 302
  };
  static std::unique_ptr<Response> Parse(const std::string& raw_response);
  void SetStatus(const ResponseCode response_code);
  void AddHeader(const std::string& header_name,
                 const std::string& header_value);
  void SetBody(const std::string& body);

  // Return string version of status code, e.g., "200"
  const std::string statusCode() const;

  std::string ToString();
  std::string GetBody();
  bool GetResponseCode(int code, Response::ResponseCode& rc); 

 private:
  std::map<int, std::string> reason_phrase_ = {
      {200, "OK"},
      {404, "NOT FOUND"},
      {302, "FOUND"}
  };
  std::string first_line_;
  using Headers = std::vector<std::pair<std::string, std::string>>;
  Headers response_header_;
  std::string response_body_;
  std::string status_code_;
};

#endif
