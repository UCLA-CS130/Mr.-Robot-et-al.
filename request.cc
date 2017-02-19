#include "request.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using Headers = std::vector<std::pair<std::string, std::string>>;

// Helper function to tokenize a string
boost::tokenizer<boost::char_separator<char>> 
token_generator(const std::string s, const char* sep) {
  // Tokenize the rest_lines 
  boost::char_separator<char> separator(sep);
  boost::tokenizer<boost::char_separator<char>> tokens(s, separator);
  return tokens;
}

std::unique_ptr<Request> Request::Parse(const std::string& raw_request) {
  std::unique_ptr<Request> req = std::unique_ptr<Request>(new Request);
  req->raw_request_ = raw_request;

  // No support for POST requests yet, so set body_ to empty string
  req->body_ = "";

  std::string first_line(raw_request.begin(), std::find(raw_request.begin(), 
                         raw_request.end(), '\r\n'));
  std::string rest_lines(std::find(raw_request.begin(), 
                         raw_request.end(), '\r\n'), raw_request.end());

  // Replace '\r\n' with space for tokenizing later
  boost::replace_all(first_line, "\r\n", "");
  boost::replace_all(rest_lines, "\r\n", "~~~");
  boost::replace_all(rest_lines, ": ", "~~~");

  // Generate tokens
  // first line depends on " " as separatorrs
  // rest of lines depends on ": " and "\r\n" as separators for key/value
  boost::tokenizer<boost::char_separator<char>> tokens1 = token_generator(
                                                          first_line, " ");
  boost::tokenizer<boost::char_separator<char>> tokens2 = token_generator(
                                                          rest_lines, "~~~");

  // Tokenize first_line
  // We assume the 2nd token is the uri: GET /path/to/resource.txt
  int i = 0;
  auto curToken1 = tokens1.begin();
  for (;;) {
    if (curToken1 == tokens1.end()) {
      // TODO: tokens is too short
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      break;
    }
    if (i == 0) {
      // set method_ as first token
      req->method_ = *curToken1;
    }
    else if (i == 1) {
      // set uri_ as second token
      req->uri_ = *curToken1;
    }
    else if (i == 2) {
      // set version_ as third token
      req->version_ = *curToken1;
    }
    i++;
    curToken1++;
  }

  // Tokenize rest_lines
  i = 0;
  std::string headerKey = "";
  auto curToken2 = tokens2.begin();
  for (;;) {
    if (curToken2 == tokens2.end()) {
      // TODO: tokens is too short
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      break;
    }
    // add key-value pairs into headers_
    if (i % 2 == 0) {
      headerKey = *curToken2;
    }
    if (i % 2 == 1) {
      req->headers_.push_back(std::make_pair(headerKey, *curToken2));
    }
    i++;
    curToken2++;
  }

  return req;

}

std::string Request::raw_request() const {
  return raw_request_;
}

std::string Request::method() const {
  return method_;
}

std::string Request::uri() const {
  return uri_;
}

std::string Request::version() const {
  return version_;
}

Headers Request::headers() const {
  return headers_;
}

std::string Request::body() const {
  return body_;
}
