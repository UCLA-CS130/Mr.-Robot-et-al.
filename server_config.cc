#include "server_config.h"

#include <iostream>
#include <string>

ServerConfig::ServerConfig(NginxConfig config) {
  std::vector<std::string> basePath = {};
  fillOutMap(config, basePath);
  printPropertiesMap();
}

// TODO: is this a terrible function name?
// Helper function to concatenate multi-word config parameter names
//
// NginxConfigStatements contain a vector of strings which it calls 'tokens'
// when reading the tokens of a NginxConfig, an example vector of tokens
// could be ["location", "/echo"] and this function will output a string that
// is the concatenatenation of them with a space ("location /echo"). Another
// time this will be used is if for example we allow parameters like
// 'file root /home/files', then the property lookup could provide
// ["file root"] as the lookup vector to propertyLookUp().
//
// valueSize represents the number of tokens that should be interpreted as
// the 'value' of a (vector<string>, string) pair. For example in:
//   'filePath /home/files'
// valueSize would be 1 if '/home/files' is the value we want to get from
// our unordered_map. If the tokens passed in consist of something like the
// following, then valueSize could be 2 to get '/home/etc foo.gif'
//   'pathAndFileName /home/etc foo.gif'
std::string buildWordyParam(std::shared_ptr<NginxConfigStatement> statement,
                            size_t valueSize) {
  size_t numTokens = statement->tokens_.size();
  std::string paramName = "";

  for (size_t i = 0; i < numTokens-valueSize; i++) {
    if (paramName != "")
      paramName += " ";
    paramName += statement->tokens_[i];
  }

  return paramName;
}

// In NgnixConfig, Statements are root level 'blocks' (e.g. server {})
// fillOutMap will recursively construct an unordered_map between a vector of strings
// representing the 'path' to a config parameter and the value of that parameter
//   e.g. ["server", "listen"] should be mapped to the port number
void ServerConfig::fillOutMap(NginxConfig config, std::vector<std::string> basePath) {
  if (config.statements_.size() < 1) {
    // TODO: Possible logging here about empty config
    return;
  }

  // Loop through all config param block at the current depth
  for (size_t i = 0; i < config.statements_.size(); i++) {
    size_t numTokens = config.statements_[i]->tokens_.size();
    if (numTokens < 1) {
      std::cout << "Invalid statement: no value associated with param\n";
      std::cout << config.statements_[i]->ToString(5) << std::endl;
      return;
    }
    else if (config.statements_[i]->child_block_ != nullptr) {
      // Dealing with an NginxConfig Block
      std::vector<std::string> basePathExtended = basePath;
      basePathExtended.push_back(buildWordyParam(config.statements_[i], 0));
      fillOutMap(*(config.statements_[i]->child_block_), basePathExtended);
    }
    else {
      // Dealing with an NginxConfigStatement
      // LeafTokens denote the number of 'words' in a statement w/o a child block
      size_t numLeafTokens = config.statements_[i]->tokens_.size();
      std::vector<std::string> finalPath = basePath;
      finalPath.push_back(buildWordyParam(config.statements_[i], 1));
      path_to_values_[finalPath] = config.statements_[i]->tokens_[numLeafTokens-1];
    }
  }
}

// Print the contents of the mapping of 'path to config param' -> value
void ServerConfig::printPropertiesMap() {
  // Iterate over an unordered_map using range based for loop
  std::cout << "Mappings in property_to_values_:\n" << std::endl;
  for (std::pair<std::vector<std::string>, std::string> element : path_to_values_) {
    for (auto const& key : element.first) {
      std::cout << key << ".";
    }
    std::cout << " -> " << element.second << std::endl;
  }
}

// The outward facing interface of ServerConfig, returns an int representing the
// status of the call and sets in *val, the value of the property passed in.
// TODO: check return val? returns 0 on success, nonzero on any error
int ServerConfig::propertyLookUp(const std::vector<std::string>& propertyPath,
                                 std::string& val) const {
  // unordered_map::at(key) throws if key not found.
  // unordered_map::operator[key] will silently create that entry
  // See: http://www.cplusplus.com/reference/unordered_map/unordered_map/operator[]/
  // and: http://www.cplusplus.com/reference/stdexcept/out_of_range/
  try {
    // TODO: add logging output here
    val = path_to_values_.at(propertyPath);
    return 0;
  }
  catch (const std::out_of_range& oor) {
    // TODO: add logging output here
    // TODO: give some error code which the handlers can interpret to return 404,etc.
    return 1;
  }
}
