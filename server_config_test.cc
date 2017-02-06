#include "config_parser.h"
#include "gtest/gtest.h"
#include "request_handlers.h"
#include "server_config.h"

#include <string>
#include <sstream>

class ServerConfigTest : public ::testing::Test {
protected:
  // Helper method to pass arbitrary strings into the parser
  std::unique_ptr<ServerConfig> ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    parser_.Parse(&config_stream, &out_config_);

    return std::unique_ptr<ServerConfig>(new ServerConfig(out_config_));
  }

  NginxConfigParser parser_;
  NginxConfig out_config_;
};

TEST_F(ServerConfigTest, NoPortConfig) {
    std::unique_ptr<ServerConfig> server_config = ParseString("server { ACM Hack; }");
    std::vector<std::string> query = {"server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(1, ec)
      << "Error value of 1 should be returned if property isn't found";
    EXPECT_EQ("", port)
      << "No port should be found without a 'listen' property";
}

TEST_F(ServerConfigTest, PortNumberMissing) {
    std::unique_ptr<ServerConfig> server_config = ParseString("listen;");
    std::vector<std::string> query = {"server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(1, ec)
      << "Error value of 1 should be returned if property isn't found";
    EXPECT_EQ("", port)
      << "A listen property without a port number should not return a port";
}

TEST_F(ServerConfigTest, PortPropertyOnly) {
    std::unique_ptr<ServerConfig> server_config = ParseString("listen 1234;");
    std::vector<std::string> query = {"listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("1234", port)
      << "A listen-portnumber pair should yield a port number";
}

TEST_F(ServerConfigTest, NestedPortNumberMissing) {
    std::unique_ptr<ServerConfig> server_config = ParseString("server { listen; }");
    std::vector<std::string> query = {"server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(1, ec)
      << "Error value of 1 should be returned if property isn't found with a value";
    EXPECT_EQ("", port)
      << "A listen property without a port number in a nested block should \
          give an empty lookup result";
}

TEST_F(ServerConfigTest, NonsenseProperty) {
    std::unique_ptr<ServerConfig> server_config = ParseString("listen 1234;");
    std::vector<std::string> query = {"laksdj3"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(1, ec)
      << "A property lookup on a nonsense property should return 1 (meaning error)";
}

TEST_F(ServerConfigTest, SingleNestedPort) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { listen 1234; }");
    std::vector<std::string> query = {"server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a nested block should \
          yield a port number";
}

TEST_F(ServerConfigTest, DoublyNestedPort) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("outer { server { listen 1234; } }");
    std::vector<std::string> query = {"outer", "server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a doubly nested block \
          should yield a port number";
}

TEST_F(ServerConfigTest, NonPortPropertiesPresent) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { echo true; listen 1234; }");
    std::vector<std::string> query = {"server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a doubly nested block with \
          other properties should yield a port number";
}

TEST_F(ServerConfigTest, HighlyNestedPropertiesSet) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("outer { server { echo true; listen 1234; } }");
    std::vector<std::string> query = {"outer", "server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a triply nested block with \
          other properties should yield a port number";
}

TEST_F(ServerConfigTest, PropertiesSetWithNewlines) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { echo \ntrue; listen \n1234; }");
    std::vector<std::string> query = {"server", "listen"};
    std::string port = "";
    int ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a nested block with \
          other properties and newlines should yield a port number";
}

TEST_F(ServerConfigTest, MultiWordProperties) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { \
                       echo \ntrue; \
                       listen \n1234; \
                       location /echo { \
                         action echo; \
                       } \
                     }");
    std::vector<std::string> query = {"server", "location /echo", "action"};
    std::string action = "";
    int ec = server_config->propertyLookUp(query, action);

    EXPECT_EQ(0, ec)
      << "Error value of 0 should be returned if property is found";
    EXPECT_EQ("echo", action)
      << "Requesting the action property should yield echo in this config.";
}

