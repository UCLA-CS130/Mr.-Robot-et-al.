#include "request.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

boost::tokenizer<boost::char_separator<char>> 
Request::token_generator(std::string s, const std::string separator) {
  // Tokenize the rest_lines 
  boost::char_separator<char> sep(separator);
  boost::tokenizer<boost::char_separator<char>> tokens(s, sep);
  return tokens;
}

static unique_ptr<Request> Request::Parse(const std::string& raw_request) {
  raw_request_ = raw_request;
  // No support for POST requests yet, so set body_ to empty string
  body_ = "";

  std::string first_line(raw_request.begin(), std::find(raw_request.begin(), 
                         raw_request.end(), '\r\n'));
  std::string rest_lines(std::find(raw_request.begin(), 
                         raw_request.end(), '\r\n'), raw_request.end());

  // Replace '\r\n' with space for tokenizing later
  boost::replace_all(first_lines, "\r\n", "");
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
  auto curToken = tokens1.begin();
  for (;;) {
    if (curToken == tokens1.end()) {
      // TODO: tokens is too short
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      break;
    }
    if (i == 0) {
      // set method_ as first token
      method_ = *curToken;
    }
    else if (i == 1) {
      // set uri_ as second token
      uri_ = *curToken;
    }
    else if (i == 2) {
      // set version_ as third token
      version_ = *curToken;
    }
    i++;
    curToken++;
  }

  // Tokenize rest_lines
  i = 0;
  std::string headerKey = "";
  auto curToken = tokens2.begin();
  for (;;) {
    if (curToken == tokens2.end()) {
      // TODO: tokens is too short
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      break;
    }
    // add key-value pairs into headers_
    if (i % 2 == 0) {
      headerKey = *curToken;
    }
    if (i % 2 == 1) {
      headers_.push_back(std::make_pair(headerKey, *curToken));
    }
    i++;
    curToken++;
  }

  return std::unique_ptr<Request>(this);

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

Headers Request:headers() const {
  return headers_;
}

std::string Request::body() const {
  return body_;
}

