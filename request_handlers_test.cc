#include "gtest/gtest.h"
#include "server_config.h"
#include "config_parser.h"
#include "request_handlers.h"
#include "mime_types.h"

#include <cstddef>
#include <sstream>
#include <fstream>

class RequestHandlersTest : public ::testing::Test {
protected:

  const size_t header_size = 45;

  bool EchoHandleRequest(const char* request_buffer,
                         const size_t& request_buffer_size,
                         char* &response_buffer,
                         size_t& response_buffer_size) {
    // Read config file
    NginxConfig config;
    NginxConfigParser config_parser_;
    std::stringstream config_stream("simple_config");
    config_parser_.Parse(&config_stream, &config);
    ServerConfig server_config(config);
    EchoRequestHandler echo_request_handler;
    echo_request_handler.handleRequest(server_config,
                                       request_buffer,
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

  bool StaticHandleRequest(const char* request_buffer,
                           const size_t& request_buffer_size,
                           char* &response_buffer,
                           size_t& response_buffer_size) {
    // Read config file
    NginxConfig config;
    NginxConfigParser config_parser_;
    std::stringstream config_stream("simple_config");
    config_parser_.Parse(&config_stream, &config);
    ServerConfig server_config(config);
    StaticRequestHandler static_request_handler;
    static_request_handler.handleRequest(server_config,
                                         request_buffer,
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

  const char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  ASSERT_TRUE(EchoHandleRequest(request_buffer,
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
  const char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nabcd\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  ASSERT_TRUE(EchoHandleRequest(request_buffer,
                                request_buffer_size,
                                response_buffer,
                                response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should only be header size plus initial request buffer";
  EXPECT_STREQ(response_buffer, expected_response_buffer)
    << "response_buffer should match expected_response_buffer";
}


TEST_F(RequestHandlersTest, OutputWithHeader) {
  const char request_buffer[] =
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
  const char test_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
    "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  ASSERT_TRUE(EchoHandleRequest(request_buffer,
                                request_buffer_size,
                                response_buffer,
                                response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(test_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
}

TEST_F(RequestHandlersTest, HTMLTest) {
  char request_buffer[] = "";
  const size_t request_buffer_size = 0;

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  const char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  // TODO: Need a way to diff two HTML files
  // TODO: Need to convert string to char array to diff
  std::ifstream file("test/index.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string contents(buffer.str());
  std::cout << contents << std::endl;

  ASSERT_TRUE(StaticHandleRequest(request_buffer,
                                  request_buffer_size,
                                  response_buffer,
                                  response_buffer_size));
}

TEST_F(RequestHandlersTest, PNGTest) {
  char request_buffer[] = "";
  const size_t request_buffer_size = 0;

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  const char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n";
  const size_t expected_response_buffer_size = header_size + request_buffer_size;

  // TODO: Need a way to diff two images; most online resources
  // say to use OpenCV
  // TODO: Need to convert string to char array to diff
  std::ifstream file("test/angrybird.png", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string contents(buffer.str());
  std::cout << contents << std::endl;

  ASSERT_TRUE(StaticHandleRequest(request_buffer,
                                  request_buffer_size,
                                  response_buffer,
                                  response_buffer_size));
}

// TEST_F(RequestHandlersTest, GIFTest) {
//   char request_buffer[] = "";
//   const size_t request_buffer_size = 0;

//   // The request_buffer is normally filled by a socket-read
//   // The response_buffer is allocated by the request-handler
//   char* response_buffer = nullptr;
//   size_t response_buffer_size = 0;

//   const char expected_response_buffer[] =
//     "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n";
//   const size_t expected_response_buffer_size = header_size + request_buffer_size;

//   // TODO: Need a way to diff two GIFs
//   ASSERT_TRUE(StaticHandleRequest(request_buffer,
//                                   request_buffer_size,
//                                   response_buffer,
//                                   response_buffer_size));
// }

