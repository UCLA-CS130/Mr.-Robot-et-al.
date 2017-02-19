#include "response.h"
#include <boost/lexical_cast.hpp>

void Response::SetStatus(const ResponseCode response_code) {
  response_header_ = "HTTP/1.1 " + boost::lexical_cast<std::string>(response_code) + "\r\n";
}

void Response::AddHeader(const std::string& header_name,
                         const std::string& header_value) {
  std::string header_line = header_name + ": " + header_value + "\r\n";
  response_header_ = response_header_ + header_line;
}

void Response::SetBody(const std::string& body) {
  response_body_ = body;
}

std::string Response::ToString() {
  return response_header_ + "\r\n" + response_body_ + "\r\n";
}