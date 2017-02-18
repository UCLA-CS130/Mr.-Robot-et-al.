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

    RequestRouter router(server_config);
    router.routeRequest("TODO");

    // TODO: Update tests for new RequestRouter/RequestHandler interface
    return true;
  }
};

TEST_F(RequestHandlersTest, EmptyStringTest) {
  const char request_buffer[] = "";
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
  const char request_buffer[] = "abcd\n";
  const size_t request_buffer_size = 5;

  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

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
  const size_t request_buffer_size = strlen(request_buffer);

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
  const size_t expected_response_buffer_size = strlen(test_response_buffer);

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
  const char request_buffer[] = "GET /static1/index.html HTTP/1.1\r\n"
                          "Host: localhost:8080\r\n"
                          "Accept-Encoding: gzip, deflate, compress\r\n"
                          "Accept: */*\r\n"
                          "User-Agent: HTTPie/0.8.0\r\n\r\n";
  const size_t request_buffer_size = strlen(request_buffer);

  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  std::ifstream file("test/index.html", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string html_content(buffer.str());

  std::string content =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n" + html_content;
  const size_t expected_response_buffer_size = content.size();

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

TEST_F(RequestHandlersTest, RouteDoesNotExist) {
  const char request_buffer[] = "GET /nowhere HTTP/1.1\r\n"
                                "Host: localhost:8080\r\n"
                                "Accept-Encoding: gzip, deflate, compress\r\n"
                                "Accept: */*\r\n"
                                "User-Agent: HTTPie/0.8.0\r\n\r\n";
  const size_t request_buffer_size = strlen(request_buffer);

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

TEST_F(RequestHandlersTest, RequestedFileDoesNotExist) {
  const char request_buffer[] = "GET /static1/nonexistantfile HTTP/1.1\r\n"
                                "Host: localhost:8080\r\n"
                                "Accept-Encoding: gzip, deflate, compress\r\n"
                                "Accept: */*\r\n"
                                "User-Agent: HTTPie/0.8.0\r\n\r\n";
  const size_t request_buffer_size = strlen(request_buffer);

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
  const char request_buffer[] = "GET /static1/angrybird.png HTTP/1.1\r\n"
                                "Host: localhost:8080\r\n"
                                "Accept-Encoding: gzip, deflate, compress\r\n"
                                "Accept: */*\r\n"
                                "User-Agent: HTTPie/0.8.0\r\n\r\n";
  const size_t request_buffer_size = strlen(request_buffer);

  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  std::ifstream file("test/angrybird.png", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string image_content(buffer.str());

  std::string content =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: image/png\r\n\r\n" + image_content;
  const size_t expected_response_buffer_size = content.size();

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

  delete[] expected_response_buffer;
}

TEST_F(RequestHandlersTest, GIFTest) {
  const char request_buffer[] = "GET /static1/angrybird.gif HTTP/1.1\r\n"
                                "Host: localhost:8080\r\n"
                                "Accept-Encoding: gzip, deflate, compress\r\n"
                                "Accept: */*\r\n"
                                "User-Agent: HTTPie/0.8.0\r\n\r\n";
  const size_t request_buffer_size = 0;

  char* response_buffer = nullptr;
  size_t response_buffer_size = 0;

  std::ifstream file("test/angrybird.gif", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string image_content(buffer.str());

  std::string content =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: image/gif\r\n\r\n" + image_content;
  const size_t expected_response_buffer_size = content.size();

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

  delete[] expected_response_buffer;
}

