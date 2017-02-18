#include "request.h"

#include <boost/tokenizer.hpp>

static unique_ptr<Request> Request::Parse(const std::string& raw_request) {
  raw_request_ = raw_request;
  // Tokenize the request so it's easy to extract components
  boost::char_separator<char> separator(" ");
  boost::tokenizer<boost::char_separator<char>> tokens(raw_request, separator);

  // We assume that files are stored at the top level and won't recursively
  // search for the requested file. Also, that routing in the config is
  // specified within the server block.
  std::vector<std::string> query = {"server"};
  // We assume the 2nd token is the resource path: GET /path/to/resource.txt
  int i = 0;
  std::string headerKey = "";
  auto curToken = tokens.begin();
  for (;;) {
    if (curToken == tokens.end()) {
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
    else {
      if (i % 2 == 1 && i > 2) {
        headerKey = *curToken;
      }
      if (i % 2 == 0 && i > 2) {
        headers_.push_back(std::make_pair(headerKey, *curToken));
      }
      // TODO: Still need to take care of filling body_ variable
    }

    i++;
    curToken++;
  }
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

