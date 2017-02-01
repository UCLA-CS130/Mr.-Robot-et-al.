#include "server_config.h"

#include <iostream>
#include <string>

ServerConfig::ServerConfig(NginxConfig config) {
  fillOutMap(config);
}

ServerConfig::~ServerConfig() {
  // TODO
}

void ServerConfig::fillOutMap(NginxConfig config) {
  // Initialize variables property, value
  std::string property = "";
  std::string value = "";
  for (size_t i = 0; i < config.statements_.size(); i++) {
    //search in child block
    if (config.statements_[i]->child_block_ != nullptr) {
      fillOutMap(*(config.statements_[i]->child_block_));
    }
    
    property = config.statements_[i]->tokens_[0];

    if (config.statements_[i]->child_block_ == nullptr) {
      value = config.statements_[i]->tokens_[1];
    }

    if (property == "listen" && value != "") {
      property_to_values_["port"] = value;
    }
  }
}

void ServerConfig::printPropertiesMap() {
  // Iterate over an unordered_map using range based for loop
  std::cout << "Mappings in property_to_values_:\n" << std::endl;
  for (std::pair<std::string, std::string> element : property_to_values_) {
    std::cout << element.first << " :: " << element.second << std::endl;
  }
}

std::string ServerConfig::propertyLookUp(std::string propertyName) {
  return property_to_values_[propertyName];
}
