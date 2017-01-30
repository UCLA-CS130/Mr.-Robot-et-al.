#include <iostream>
#include "server_config.h"
#include <string>

ServerConfig::ServerConfig(NginxConfig config) {
	fillOutMap(config);
}

ServerConfig::~ServerConfig() {
  // TODO
}

void ServerConfig::fillOutMap(NginxConfig config) {
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
	    // std::cout << property << std::endl << value << std::endl;
	    if (property == "listen")
	    	property_to_values_["port"] = value;
	}
	return;

}

void ServerConfig::printPropertiesMap() {
	// Iterate over an unordered_map using range based for loop
	for (std::pair<std::string, std::string> element : property_to_values_)
	{
		std::cout << element.first << " :: " << element.second << std::endl;
	}
}

std::string ServerConfig::propertyLookUp(std::string propertyName) {
	// this->print();
	return property_to_values_[propertyName];
}

