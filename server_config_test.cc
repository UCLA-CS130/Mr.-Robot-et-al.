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

    EXPECT_EQ("", server_config->propertyLookUp("port"))
      << "No port should be found without a 'listen' property";
}

TEST_F(ServerConfigTest, PortNumberMissing) {
    std::unique_ptr<ServerConfig> server_config = ParseString("listen;");

    EXPECT_EQ("", server_config->propertyLookUp("port"))
      << "A listen property without a port number should give an empty lookup result";
}

TEST_F(ServerConfigTest, PortPropertyOnly) {
    std::unique_ptr<ServerConfig> server_config = ParseString("listen 1234;");

    EXPECT_EQ("1234", server_config->propertyLookUp("port"))
      << "A listen-portnumber pair should yield a port number";
}

TEST_F(ServerConfigTest, NestedPortNumberMissing) {
    std::unique_ptr<ServerConfig> server_config = ParseString("server { listen; }");

    EXPECT_EQ("", server_config->propertyLookUp("port"))
      << "A listen property without a port number in a nested block should \
          give an empty lookup result";
}

TEST_F(ServerConfigTest, NonsenseProperty) {
    std::unique_ptr<ServerConfig> server_config = ParseString("listen 1234;");

    EXPECT_EQ("", server_config->propertyLookUp("kl3j2lkj"))
      << "A property lookup on a nonsense property should yield nothing";
}

TEST_F(ServerConfigTest, SingleNestedPort) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { listen 1234; }");

    EXPECT_EQ("1234", server_config->propertyLookUp("port"))
      << "A listen property with a port number in a nested block should \
          yield a port number";
}

TEST_F(ServerConfigTest, DoublyNestedPort) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("outer { server { listen 1234; } }");

    EXPECT_EQ("1234", server_config->propertyLookUp("port"))
      << "A listen property with a port number in a doubly nested block \
          should yield a port number";
}

TEST_F(ServerConfigTest, NonPortPropertiesPresent) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { echo true; listen 1234; }");

    EXPECT_EQ("1234", server_config->propertyLookUp("port"))
      << "A listen property with a port number in a doubly nested block with \
          other properties should yield a port number";
}

TEST_F(ServerConfigTest, HighlyNestedPropertiesSet) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("outer { server { echo true; listen 1234; } }");

    EXPECT_EQ("1234", server_config->propertyLookUp("port"))
      << "A listen property with a port number in a triply nested block with \
          other properties should yield a port number";
}

TEST_F(ServerConfigTest, PropertiesSetWithNewlines) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { echo \ntrue; listen \n1234; }");

    EXPECT_EQ("1234", server_config->propertyLookUp("port"))
      << "A listen property with a port number in a nested block with \
          other properties and newlines should yield a port number";
}
