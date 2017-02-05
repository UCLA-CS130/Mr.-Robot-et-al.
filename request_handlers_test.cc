#include "gtest/gtest.h"
#include "request_handlers.h"

using namespace lightning_server;

class RequestHandlersTest : public ::testing::Test {
protected:
  const size_t header_size = 45;
  bool HandleRequest(const char* request_buffer,
                     const size_t& request_buffer_size,
                     char* &response_buffer,
                     size_t& response_buffer_size) {

    request_handlers::echoRequestHandler(request_buffer,
                                         request_buffer_size,
                                         response_buffer,
                                         response_buffer_size);
    if (response_buffer == nullptr) {
      return false;
    }
    else {
      return true;
    }
  }
};

TEST_F(RequestHandlersTest, EmptyStringTest) {
  char request_buffer[] = "";
  const size_t request_buffer_size = 0;

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  ASSERT_TRUE(HandleRequest(request_buffer,
                            request_buffer_size,
                            response_buffer,
                            response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should only be header size";
  EXPECT_STREQ(response_buffer, expected_response_buffer)
    << "response_buffer should match expected_response_buffer";
}

TEST_F(RequestHandlersTest, RandomStringTest) {
  char request_buffer[] = "abcd\n";
  const size_t request_buffer_size = 5;

  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  // Variable for comparing with actual response_buffer
  char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nabcd\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  ASSERT_TRUE(HandleRequest(request_buffer,
                            request_buffer_size,
                            response_buffer,
                            response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should only be header size plus initial request buffer";
  EXPECT_STREQ(response_buffer, expected_response_buffer)
    << "response_buffer should match expected_response_buffer";
}


TEST_F(RequestHandlersTest, OutputWithHeader) {
  char request_buffer[] =
    "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  const size_t request_buffer_size = 292;

  // Handle echo response in external handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  // Compare with the following:
  char test_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
    "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  ASSERT_TRUE(HandleRequest(request_buffer,
                            request_buffer_size,
                            response_buffer,
                            response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(test_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
}
