#include "response.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>
#include <string>

std::unique_ptr<Response> Response::Parse(const std::string& raw_response) {
  std::unique_ptr<Response> parsed_response(new Response); 
  std::string headers = "";
  std::string body = "";

  // Separate headers and bocy from raw_response:
  size_t end_of_headers_index = raw_response.find("\r\n\r\n"); 
  if(end_of_headers_index == std::string::npos)
  {
    std::cout << "Unable to find headers of response.\n";
    return nullptr;
  }
  size_t begin_of_body_index = end_of_headers_index + 4; // +4 for the \r\n\r\n
  // Save body
  if (raw_response.size() > begin_of_body_index) {
    body = raw_response.substr(begin_of_body_index, raw_response.size() - begin_of_body_index);
    parsed_response->SetBody(body); 
  }

  headers = raw_response.substr(0, end_of_headers_index+1);

  // Extract headers into lines:
  std::vector<std::string> lines; 
  boost::split(lines,headers,boost::is_any_of("\r\n"));

  // Extract response line and status code
  std::vector<std::string> resp_line; 
  boost::split(resp_line,lines[0],boost::is_any_of(" "));

  if (resp_line.size() != 3) {
    std::cout << "Malformed response line.\n";
    return nullptr;
  }

  ResponseCode rc;
  std::string status_code = resp_line[1];
  if (!parsed_response->GetResponseCode(std::stoi(status_code), rc)) {
    std::cout << "Response code of response unknown " << status_code << std::endl;
    return nullptr;
  } 

  parsed_response->SetStatus(rc);

  // Handle remaining headers
  for(unsigned int i = 1; i < lines.size(); i++){
    std::string header_name;
    std::string header_value;

    if(!lines[i].empty()){
      std::size_t index = lines[i].find_first_of(":");

      if(index != std::string::npos) {
        header_name = lines[i].substr(0, index);
        header_value = lines[i].substr(index+2, std::string::npos);
      }
      parsed_response->AddHeader(header_name, header_value);
    }
  }

  return parsed_response;
}

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

bool Response::GetResponseCode(int code, Response::ResponseCode& rc) {
  switch (code) {
    case 200:
      rc = ResponseCode::OK;
      return true; 
    case 404:
      rc = ResponseCode::NOT_FOUND;
      return true;
    case 302:
      rc = ResponseCode::FOUND;
      return true; 
    default: 
      return false;
  }
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
