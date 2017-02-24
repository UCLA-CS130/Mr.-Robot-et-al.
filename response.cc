#include "response.h"

#include <boost/lexical_cast.hpp>
#include <iostream>

void Response::SetStatus(const ResponseCode response_code) {
  // boost lexical_cast will convert response_code of type int to type string
  status_code_ = boost::lexical_cast<std::string>(response_code);
  std::string reason_phrase = reason_phrase_.find(response_code)->second;
  first_line_ = "HTTP/1.1 " + status_code_ + " " + reason_phrase;
  std::cout << "Response First Line: " + first_line_ << std::endl;
}

void Response::AddHeader(const std::string& header_name,
                         const std::string& header_value) {
  response_header_.push_back(std::make_pair(header_name, header_value));
}

void Response::SetBody(const std::string& body) {
  response_body_ = body;
}

const std::string Response::statusCode() const {
  return status_code_;
}

std::string Response::ToString() {
  std::string response_headers_str = "";
  for (size_t i = 0; i < response_header_.size(); i++) {
    response_headers_str += response_header_[i].first + ": "
                            + response_header_[i].second + "\r\n";
  }
  return first_line_ + "\r\n" + response_headers_str + "\r\n" +
         response_body_;
}

std::string Response::GetBody() {
  return response_body_;
}
