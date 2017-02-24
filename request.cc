#include "request.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

// Helper function to tokenize a string
boost::tokenizer<boost::char_separator<char>>
tokenGenerator(const std::string s, const char* sep) {
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

  int pos = raw_request.find_first_of('\r\n');

  std::string first_line = raw_request.substr(0, pos);

  std::string rest_lines = raw_request.substr(pos+1);
  std::cout << "first_line: " + first_line << std::endl;
  std::cout << "rest_lines: " + rest_lines << std::endl;

  // Replace '\r\n' and ': ' with '~~~' for tokenizing later
  boost::replace_all(rest_lines, "\r\n", "~~~");
  boost::replace_all(rest_lines, ": ", "~~~");

  // Generate tokens
  // first line depends on " " as separatorrs
  // rest of lines depends on "~~~" as separators for key/value
  boost::tokenizer<boost::char_separator<char>> tokens1 = tokenGenerator(
                                                          first_line, " ");
  boost::tokenizer<boost::char_separator<char>> tokens2 = tokenGenerator(
                                                          rest_lines, "~~~");

  // Tokenize first_line
  // We assume the 2nd token is the uri: GET /path/to/resource.txt
  int i = 0;
  for (auto cur_token_first_line = tokens1.begin();
       cur_token_first_line != tokens1.end(); cur_token_first_line++) {
    if  (cur_token_first_line == tokens1.end()) {
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      break;
    }
    if (i == 0) {
      // set method_ as first token
      req->method_ =  *cur_token_first_line;
    }
    else if (i == 1) {
      // set uri_ as second token
      req->uri_ =  *cur_token_first_line;
    }
    else if (i == 2) {
      // set version_ as third token
      req->version_ =  *cur_token_first_line;
    }
    else {
      // Error, since we only expect 3 tokens from first line
      std::cout << "ERROR: malformed request with more"
                   " than 3 tokens on the first line." << std::endl;
      return nullptr;
    }
    i++;
  }

  // Tokenize rest_lines
  i = 0;
  std::string header_key = "";
  for (auto cur_token_rest_lines = tokens2.begin();
       cur_token_rest_lines != tokens2.end(); cur_token_rest_lines++) {
    // end token can't be an even number
    auto temp = cur_token_rest_lines;
    if (temp++ == tokens2.end() && i % 2 == 0) {
      std::cout << "ERROR: malformed request with mismatching tokens "
                   "in the header specifications." << std::endl;
      return nullptr;
    }
    // add key-value pairs into headers_
    if (i % 2 == 0 || i % 2 == 2) {
      header_key = *cur_token_rest_lines;
    }
    if (i % 2 == 1) {
      req->headers_.push_back(std::make_pair(header_key, *cur_token_rest_lines));
    }
    i++;
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

Request::Headers Request::headers() const {
  return headers_;
}

std::string Request::body() const {
  return body_;
}
