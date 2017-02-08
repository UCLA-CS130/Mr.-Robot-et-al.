#include "config_parser.h"
#include "gtest/gtest.h"
#include "mime_types.h"
#include "request_handlers.h"
#include "request_router.h"
#include "server_config.h"

#include <cstddef>
#include <fstream>
#include <sstream>

class RequestHandlersTest : public ::testing::Test {
protected:

  const size_t ok_header_size = 45;

  bool HandleRequest(const char* request_buffer,
                     const size_t& request_buffer_size,
                     char* &response_buffer,
                     size_t& response_buffer_size) {

    // Read config file
    NginxConfig config;
    NginxConfigParser config_parser;
    config_parser.Parse("simple_config", &config);
    ServerConfig server_config(config);

    RequestRouter router;
    return router.routeRequest(server_config,
                               request_buffer,
                               request_buffer_size,
                               response_buffer,
                               response_buffer_size);

    /*
    if (response_buffer == nullptr) {
      return false;
    }
    else {
      return true;
    }
    */
  }
};

// TODO tests:
// invalid route
// invalid file

TEST_F(RequestHandlersTest, EmptyStringTest) {
  char request_buffer[] = "";
  const size_t request_buffer_size = 0;

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  const char expected_response_buffer[] = "";
  const size_t expected_response_buffer_size = 0;

  ASSERT_FALSE(HandleRequest(request_buffer,
                             request_buffer_size,
                             response_buffer,
                             response_buffer_size));


  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should only be header size";
  EXPECT_STREQ(response_buffer, NULL)
    << "response_buffer should match expected_response_buffer";
}

TEST_F(RequestHandlersTest, RandomStringTest) {
  char request_buffer[] = "abcd\n";
  const size_t request_buffer_size = 5;

  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  // Variable for comparing with actual response_buffer
  const char expected_response_buffer[] = "";
  const size_t expected_response_buffer_size = 0;

  ASSERT_FALSE(HandleRequest(request_buffer,
                             request_buffer_size,
                             response_buffer,
                             response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should only be header size plus initial request buffer";
  EXPECT_STREQ(response_buffer, NULL)
    << "response_buffer should match expected_response_buffer";
}

TEST_F(RequestHandlersTest, OutputWithHeader) {
  const char request_buffer[] =
    "GET /echo HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  const size_t request_buffer_size = 296;

  // Handle echo response in external handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  // Compare with the following:
  const char test_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
    "GET /echo HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  const size_t expected_response_buffer_size = ok_header_size + request_buffer_size;

  ASSERT_TRUE(HandleRequest(request_buffer,
                            request_buffer_size,
                            response_buffer,
                            response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(test_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
}

TEST_F(RequestHandlersTest, HTMLTest) {
  char request_buffer[] = "GET /static1/index.html HTTP/1.1\r\n"
                          "Host: localhost:8080\r\n"
                          "Accept-Encoding: gzip, deflate, compress\r\n"
                          "Accept: */*\r\n"
                          "User-Agent: HTTPie/0.8.0\r\n\r\n";

  const size_t request_buffer_size = strlen(request_buffer);

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  const char expected_response_buffer[] =
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\n<html>\n"
    "  <head></head>\n  <body>\n    <h1>Hello World!</h1>\n  </body>\n</html>";
  const size_t expected_response_buffer_size = 135;

  // TODO: Need a way to diff two HTML files
  // TODO: Need to convert string to char array to diff
  /*
  std::ifstream file("test/index.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string contents(buffer.str());
  std::cout << contents << std::endl;
  */

  ASSERT_TRUE(HandleRequest(request_buffer,
                            request_buffer_size,
                            response_buffer,
                            response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(expected_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
}

TEST_F(RequestHandlersTest, RouteDoesntExist) {
  char request_buffer[] = "GET /nowhere HTTP/1.1\r\n"
                          "Host: localhost:8080\r\n"
                          "Accept-Encoding: gzip, deflate, compress\r\n"
                          "Accept: */*\r\n"
                          "User-Agent: HTTPie/0.8.0\r\n\r\n";

  const size_t request_buffer_size = strlen(request_buffer);

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  const char expected_response_buffer[] =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html\r\n\r\n"

    "<html>\n"
    "<head>\n"
    "<title>Not Found</title>\n"
    "<h1>404 Page Not Found</h1>\n\n"
    "</head>\n"
    "</html>";

  const size_t expected_response_buffer_size = strlen(expected_response_buffer);

  ASSERT_FALSE(HandleRequest(request_buffer,
                             request_buffer_size,
                             response_buffer,
                             response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(expected_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
}

TEST_F(RequestHandlersTest, RequestedFileDoesntExist) {
  char request_buffer[] = "GET /static1/nonexistantfile HTTP/1.1\r\n"
                          "Host: localhost:8080\r\n"
                          "Accept-Encoding: gzip, deflate, compress\r\n"
                          "Accept: */*\r\n"
                          "User-Agent: HTTPie/0.8.0\r\n\r\n";

  const size_t request_buffer_size = strlen(request_buffer);

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  const char expected_response_buffer[] =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html\r\n\r\n"

    "<html>\n"
    "<head>\n"
    "<title>Not Found</title>\n"
    "<h1>404 Page Not Found</h1>\n\n"
    "</head>\n"
    "</html>";

  const size_t expected_response_buffer_size = strlen(expected_response_buffer);

  ASSERT_FALSE(HandleRequest(request_buffer,
                             request_buffer_size,
                             response_buffer,
                             response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(expected_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
}

TEST_F(RequestHandlersTest, PNGTest) {
  char request_buffer[] = "GET /static1/angrybird.png HTTP/1.1\r\n"
                          "Host: localhost:8080\r\n"
                          "Accept-Encoding: gzip, deflate, compress\r\n"
                          "Accept: */*\r\n"
                          "User-Agent: HTTPie/0.8.0\r\n\r\n";

  const size_t request_buffer_size = strlen(request_buffer);

  // The request_buffer is normally filled by a socket-read
  // The response_buffer is allocated by the request-handler
  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  // TODO: Need a way to diff two images; most online resources
  // say to use OpenCV
  // TODO: Need to convert string to char array to diff
  std::ifstream file("test/angrybird.png", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string image_content(buffer.str());
  // std::cout << image_content << std::endl;

  std::string content = 
  "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n" + image_content;
  size_t expected_response_buffer_size = content.size();

  char* expected_response_buffer = new char[expected_response_buffer_size];
  content.copy(expected_response_buffer, expected_response_buffer_size);

  ASSERT_TRUE(HandleRequest(request_buffer,
                            request_buffer_size,
                            response_buffer,
                            response_buffer_size));

  EXPECT_EQ(response_buffer_size, expected_response_buffer_size)
    << "Response size should be initial request buffer size plus header size";
  EXPECT_STREQ(expected_response_buffer, response_buffer)
    << "response_buffer should be the same string as test_response_buffer";
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

