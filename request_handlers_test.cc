#include "request_handlers.h"
#include "gtest/gtest.h"

#include <iostream>

using namespace lightning_server;

const int MAX_REQ_SIZE = 8192; // bytes

class RequestHandlersTest : public ::testing::Test {
protected:
  // Helper method to pass arbitrary strings into the parser
  bool HandleRequest(char* request_buffer,
                     char* response_buffer,
                     size_t& response_buffer_size) {
    request_handlers::echoRequestHandler(request_buffer,
                                         response_buffer,
                                         response_buffer_size);
    if (response_buffer == nullptr) {
      return false;
    } else {
      return true;
    }
  }
};

TEST_F(RequestHandlersTest, SimpleTest) {
  char request_buffer[MAX_REQ_SIZE];

  // Customize message in request buffer
  strcpy(request_buffer, 
     "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
     "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
     " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
     "text/html,application/xhtml+xml,application/xml;"
     "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
     "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n");

  // Handle echo response in external handler
  char response_buffer[MAX_REQ_SIZE];
  size_t response_size = strlen(request_buffer);

  EXPECT_TRUE(HandleRequest(request_buffer, response_buffer, response_size));
}

TEST_F(RequestHandlersTest, OutputWithHeader) {
  char request_buffer[MAX_REQ_SIZE];

  // Customize message in request buffer
  strcpy(request_buffer, 
     "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
     "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
     " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
     "text/html,application/xhtml+xml,application/xml;"
     "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
     "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n");

  // Handle echo response in external handler
  char response_buffer[MAX_REQ_SIZE];
  size_t response_size = 292;

  // Compare with the following:
  char test_response_buffer[MAX_REQ_SIZE];
  strcpy(test_response_buffer, 
          "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
          "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
          "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
          " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
          "text/html,application/xhtml+xml,application/xml;"
          "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
          "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n");
  size_t test_response_size = 292 + 45;
  HandleRequest(request_buffer, response_buffer, response_size);
  std::cout << "DEBUG DEBUG DEBUG" << std::endl;
  std::cout << response_size << std::endl;
  for(size_t i = 0; response_buffer[i] != '\0'; i++)
  {
     printf("%c", response_buffer[i]);
  }
  std::cout << "TRUE TRUE TRUE" << std::endl;
  std::cout << test_response_size << std::endl;
  for(size_t i = 0; test_response_buffer[i] != '\0'; i++)
  {
     printf("%c", test_response_buffer[i]);
  }
  EXPECT_EQ(test_response_buffer, response_buffer);
  EXPECT_EQ(test_response_size, response_size);
}