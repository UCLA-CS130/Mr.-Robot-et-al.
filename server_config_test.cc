#include "config_parser.h"
#include "gtest/gtest.h"
#include "request_handlers.h"
#include "server_config.h"

#include <string>
#include <sstream>
#include <unordered_map>

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

TEST_F(ServerConfigTest, BuildOK) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("port 8080; \
                     path /echo EchoHandler {}");
    bool ec = server_config->Build();

    EXPECT_EQ(true, ec)
      << "Build() should return true if a valid config is supplied to \
          a ServerConfig object";
}

TEST_F(ServerConfigTest, BuildFailed) {
    std::unique_ptr<ServerConfig> server_config = ParseString("");
    bool ec = server_config->Build();

    EXPECT_EQ(false, ec)
      << "Build() should return false if an empty config is supplied";
}

TEST_F(ServerConfigTest, AllPathsNotEmpty) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("port 8080; \
                     path /echo EchoHandler {} \
                     default NotFoundHandler {}");
    bool ec = server_config->Build();
    EXPECT_EQ(true, ec)
      << "Build() should return true if a valid config is supplied";

    std::unordered_map<std::string, std::string> paths = server_config->allPaths();
    EXPECT_EQ(false, paths.empty())
      << "allPaths() shouldn't return an empty map";
    EXPECT_EQ(2, paths.size())
      << "allPaths() should have 2 mappings, one for EchoHandler, and \
          one for the NotFoundHandler";
}

TEST_F(ServerConfigTest, AllPathsEmpty) {
    std::unique_ptr<ServerConfig> server_config = ParseString("port 8080;");
    bool ec = server_config->Build();
    EXPECT_EQ(true, ec)
      << "Build() should return true if a valid config is supplied";

    std::unordered_map<std::string, std::string> paths = server_config->allPaths();
    EXPECT_EQ(true, paths.empty())
      << "allPaths() should return an empty map";
}

TEST_F(ServerConfigTest, HasChildBlock) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("port 8080; \
                     path / StaticHandler { \
                       root /foo/bar \
                     }");
    bool ec = server_config->Build();
    EXPECT_EQ(true, ec)
      << "Build() should return true if a valid config is supplied";

    std::unique_ptr<NginxConfig> childBlock = server_config->getChildBlock("/");
    EXPECT_EQ(1, childBlock->statements_.size())
      << "The child block of '/' prefix should have one statement in it";
}

TEST_F(ServerConfigTest, EmptyChildBlock) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("port 8080; \
                     path / StaticHandler {}");
    bool ec = server_config->Build();
    EXPECT_EQ(true, ec)
      << "Build() should return true if a valid config is supplied";

    std::unique_ptr<NginxConfig> childBlock = server_config->getChildBlock("/");
    EXPECT_EQ(0, childBlock->statements_.size())
      << "The child block of '/' prefix should have no statements";
}

TEST_F(ServerConfigTest, NoPortConfig) {
    std::unique_ptr<ServerConfig> server_config = ParseString("server { ACM Hack; }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"server", "port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(false, ec)
      << "LookUp() should return false if a property isn't found";
    EXPECT_EQ("", port)
      << "No port should be found without a 'port' property";
}

TEST_F(ServerConfigTest, PortNumberMissing) {
    std::unique_ptr<ServerConfig> server_config = ParseString("port;");
    bool buildCode = server_config->Build();
    EXPECT_EQ(false, buildCode)
      << "Build() should return false when given a config property has no value.";

    std::vector<std::string> query = {"port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(false, ec)
      << "LookUp() should return false if a property doesn't have a value";
    EXPECT_EQ("", port)
      << "A listen property without a port number should not return a port";
}

TEST_F(ServerConfigTest, PortPropertyOnly) {
    std::unique_ptr<ServerConfig> server_config = ParseString("port 1234;");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(true, ec)
      << "LookUp() should return true if property is found";
    EXPECT_EQ("1234", port)
      << "A listen-portnumber pair should yield a port number";
}

TEST_F(ServerConfigTest, NestedPortNumberMissing) {
    std::unique_ptr<ServerConfig> server_config = ParseString("server { port; }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(false, buildCode)
      << "Build() should return false when given a property has no value";

    std::vector<std::string> query = {"server", "port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(false, ec)
      << "LookUp() should return false if a nested property doesn't have a value";
    EXPECT_EQ("", port)
      << "A listen property without a port number in a nested block should \
          give an empty lookup result";
}

TEST_F(ServerConfigTest, NonsenseQueryPath) {
    std::unique_ptr<ServerConfig> server_config = ParseString("port 1234;");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"laksdj3"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(false, ec)
      << "A property lookup on a nonsense property should return false";
}

TEST_F(ServerConfigTest, SingleNestedPort) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { port 1234; }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"server", "port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(true, ec)
      << "LookUp() should return true if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a nested block should \
          yield a port number";
}

TEST_F(ServerConfigTest, DoublyNestedPort) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("outer { server { port 1234; } }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> first_query = {"outer", "server", "port"};
    std::string first_port = "";
    bool first_ec = server_config->propertyLookUp(first_query, first_port);
    EXPECT_EQ(true, first_ec)
      << "LookUp() should return true if property is found";
    EXPECT_EQ("1234", first_port)
      << "A listen property with a port number in a doubly nested block \
          should yield a port number";

    std::vector<std::string> second_query = {"server", "port"};
    std::string second_port = "";
    bool second_ec = server_config->propertyLookUp(second_query, second_port);
    EXPECT_EQ(false, second_ec)
      << "LookUp() should fail if the property isn't found";
    EXPECT_EQ("", second_port)
      << "A listen property with a port number in a doubly nested block \
          shouldn't yield a port number if the wrong query is supplied";
}

TEST_F(ServerConfigTest, NonPortPropertiesPresent) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { echo true; port 1234; }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"server", "port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(true, ec)
      << "LookUp() should return true if property is found, even if it's not \
          the first property in a block";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a doubly nested block with \
          other properties should yield a port number";
}

TEST_F(ServerConfigTest, HighlyNestedPropertiesSet) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("outer { server { echo true; port 1234; } }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"outer", "server", "port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(true, ec)
      << "LookUp() should return true if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a triply nested block with \
          other properties should yield a port number";
}

TEST_F(ServerConfigTest, PropertiesSetWithNewlines) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { echo \ntrue; port \n1234; }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"server", "port"};
    std::string port = "";
    bool ec = server_config->propertyLookUp(query, port);

    EXPECT_EQ(true, ec)
      << "LookUp() should return true if property is found";
    EXPECT_EQ("1234", port)
      << "A listen property with a port number in a nested block with \
          other properties and newlines should yield a port number";
}

TEST_F(ServerConfigTest, MultiWordProperties) {
    std::unique_ptr<ServerConfig> server_config
      = ParseString("server { \
                       echo \ntrue; \
                       port \n1234; \
                       location /echo { \
                         action echo; \
                       } \
                     }");
    bool buildCode = server_config->Build();
    EXPECT_EQ(true, buildCode)
      << "Build() should return true when given a valid config";

    std::vector<std::string> query = {"server", "location /echo", "action"};
    std::string action = "";
    bool ec = server_config->propertyLookUp(query, action);

    EXPECT_EQ(true, ec)
      << "LookUp() should return true if property is found";
    EXPECT_EQ("echo", action)
      << "Requesting the action property should yield echo in this config.";
}

