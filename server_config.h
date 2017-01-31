#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "config_parser.h"
#include <unordered_map>

class ServerConfig {
  public:
  	ServerConfig(NginxConfig config);
    ~ServerConfig();
    std::string propertyLookUp(std::string propertyName);

  private:
  	void printPropertiesMap();
  	void fillOutMap(NginxConfig config);
  	std::unordered_map<std::string,std::string> property_to_values_;
};

#endif