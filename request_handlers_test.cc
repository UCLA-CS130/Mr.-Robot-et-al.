#include "config_parser.h"
#include "gtest/gtest.h"
#include "mime_types.h"
#include "request_handlers.h"
#include "request_router.h"
#include "server_config.h"
#include "request.h"
#include "response.h"

#include <cstddef>
#include <fstream>
#include <sstream>

class RequestHandlersTest : public ::testing::Test {
protected:
  NginxConfig config;
  NginxConfigParser config_parser;
  ServerConfig server_config;

  Request request;
  Response response;

  bool prepareRequest(const std::string raw_request) {
    // Read config file
    config_parser.Parse("simple_config", &config);
    server_config = ServerConfig(config);
    server_config.build();

    request = *(Request::Parse(raw_request));

    // TODO: Update tests for new RequestRouter/RequestHandler interface
    return true;
  }
};

TEST_F(RequestHandlersTest, EchoHandlerTest) {
  const std::string raw_request =
    "GET /echo HTTP/1.1\r\nHost: 127.0.0.1:2020\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  ASSERT_TRUE(prepareRequest(raw_request));

  EchoRequestHandler echo_handler;
  bool init_ec = echo_handler.init("/echo", config);
  EXPECT_EQ(init_ec, true)
    << "Echo handler should correctly initialize using 'simple_config'";

  RequestHandler::Status status = echo_handler.handleRequest(request, &response);
  EXPECT_EQ(status, RequestHandler::OK)
    << "Echo handler should have an 'OK' status code for this simple test";

  const std::string test_response_string =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
    "GET /echo HTTP/1.1\r\nHost: 127.0.0.1:2020\r\n"
    "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64;"
    " rv:44.0) Gecko/20100101 Firefox/44.0\r\nAccept: "
    "text/html,application/xhtml+xml,application/xml;"
    "q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
  const std::string response_string = response.ToString();
  EXPECT_EQ(response_string, test_response_string)
    << "response_string should be the same string as test_response_buffer";
}

TEST_F(RequestHandlersTest, StaticHandlerHTMLTest) {
  const std::string raw_request =
    "GET /static/index.html HTTP/1.1\r\n"
    "Host: localhost:2020\r\n"
    "Accept-Encoding: gzip, deflate, compress\r\n"
    "Accept: */*\r\n"
    "User-Agent: HTTPie/0.8.0\r\n\r\n";
  ASSERT_TRUE(prepareRequest(raw_request));

  StaticRequestHandler static_handler;
  bool init_ec = static_handler.init("/static",
                                     *(server_config.getChildBlock("/static")));
  EXPECT_EQ(init_ec, true)
    << "Static handler should correctly initialize using 'simple_config'";

  RequestHandler::Status status = static_handler.handleRequest(request, &response);
  EXPECT_EQ(status, RequestHandler::OK)
    << "Static handler should have an 'OK' status code for this simple test of \
        getting the index.html test file";

  // Getting the content of index.html
  std::ifstream file("test/index.html", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string html_content(buffer.str());

  const std::string response_body = response.GetBody();
  EXPECT_EQ(response_body, html_content)
    << "The response body should match the content of index.html";
}

TEST_F(RequestHandlersTest, StaticHandlerFileNotFoundTest) {
  const std::string raw_request =
    "GET /static/foo.bar HTTP/1.1\r\n"
    "Host: localhost:2020\r\n"
    "Accept-Encoding: gzip, deflate, compress\r\n"
    "Accept: */*\r\n"
    "User-Agent: HTTPie/0.8.0\r\n\r\n";
  ASSERT_TRUE(prepareRequest(raw_request));

  StaticRequestHandler static_handler;
  bool init_ec = static_handler.init("/static",
                                     *(server_config.getChildBlock("/static")));
  EXPECT_EQ(init_ec, true)
    << "Static handler should correctly initialize using 'simple_config'";

  RequestHandler::Status status = static_handler.handleRequest(request, &response);
  EXPECT_EQ(status, RequestHandler::NOT_FOUND)
    << "Static handler should have a 'NOT_FOUND' status code for this test of \
        request of non-existant resource";
}

TEST_F(RequestHandlersTest, StaticHandlerPNGTest) {
  const std::string raw_request =
    "GET /static/angrybird.png HTTP/1.1\r\n"
    "Host: localhost:2020\r\n"
    "Accept-Encoding: gzip, deflate, compress\r\n"
    "Accept: */*\r\n"
    "User-Agent: HTTPie/0.8.0\r\n\r\n";
  ASSERT_TRUE(prepareRequest(raw_request));

  StaticRequestHandler static_handler;
  bool init_ec = static_handler.init("/static",
                                     *(server_config.getChildBlock("/static")));
  EXPECT_EQ(init_ec, true)
    << "Static handler should correctly initialize using 'simple_config'";

  RequestHandler::Status status = static_handler.handleRequest(request, &response);
  EXPECT_EQ(status, RequestHandler::OK)
    << "Static handler should have an 'OK' status code for getting the \
        getting the angrybird.png image";

  // Getting the content of angrybird.png
  std::ifstream file("test/angrybird.png", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string img_content(buffer.str());

  const std::string response_body = response.GetBody();
  EXPECT_EQ(response_body, img_content)
    << "The response body should match the content of angrybird.png";
}

TEST_F(RequestHandlersTest, StaticHandlerGIFTest) {
  const std::string raw_request =
    "GET /static/angrybird.gif HTTP/1.1\r\n"
    "Host: localhost:2020\r\n"
    "Accept-Encoding: gzip, deflate, compress\r\n"
    "Accept: */*\r\n"
    "User-Agent: HTTPie/0.8.0\r\n\r\n";
  ASSERT_TRUE(prepareRequest(raw_request));

  StaticRequestHandler static_handler;
  bool init_ec = static_handler.init("/static",
                                     *(server_config.getChildBlock("/static")));
  EXPECT_EQ(init_ec, true)
    << "Static handler should correctly initialize using 'simple_config'";

  RequestHandler::Status status = static_handler.handleRequest(request, &response);
  EXPECT_EQ(status, RequestHandler::OK)
    << "Static handler should have an 'OK' status code for getting the \
        getting the angrybird.gif image";

  // Getting the content of angrybird.gif
  std::ifstream file("test/angrybird.gif", std::ios::binary);
  std::ostringstream buffer;
  buffer << file.rdbuf();
  std::string img_content(buffer.str());

  const std::string response_body = response.GetBody();
  EXPECT_EQ(response_body, img_content)
    << "The response body should match the content of angrybird.gif";
}

TEST_F(RequestHandlersTest, NotFoundHandlerTest) {
  const std::string raw_request =
    "GET /nowhere HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
    "Accept-Encoding: gzip, deflate, compress\r\n"
    "Accept: */*\r\n"
    "User-Agent: HTTPie/0.8.0\r\n\r\n";
  ASSERT_TRUE(prepareRequest(raw_request));

  NotFoundRequestHandler not_found_handler;
  bool init_ec = not_found_handler.init("default",
                                        *(server_config.getChildBlock("default")));
  EXPECT_EQ(init_ec, true)
    << "Not found handler should correctly initialize using 'simple_config'";

  RequestHandler::Status status = not_found_handler.handleRequest(request, &response);
  EXPECT_EQ(status, RequestHandler::NOT_FOUND)
    << "Not found handler should have a 'NOT_FOUND' status code";

  const std::string expected_response =
    "HTTP/1.1 404 NOT FOUND\r\n"
    "Content-Type: text/html\r\n\r\n"

    "<html>\n"
    "<head>\n"
    "<title>Not Found</title>\n"
    "<h1>404 Page Not Found</h1>\n\n"
    "</head>\n"
    "</html>";

  const std::string response_string = response.ToString();
  EXPECT_EQ(response_string, expected_response)
    << "The resulting response should match the expected 404 response";
}

