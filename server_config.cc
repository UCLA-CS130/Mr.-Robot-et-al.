#include "server_config.h"

#include <iostream>
#include <string>

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
                            size_t value_size) {
  size_t num_tokens = statement->tokens_.size();
  std::string param_name = "";

  for (size_t i = 0; i < num_tokens - value_size; i++) {
    if (param_name != "")
      param_name += " ";
    param_name += statement->tokens_[i];
  }

  return param_name;
}

// ServerConfig acts as a wrapper for NginxConfig objects, allowing for easier
// retrieval of things like port, child blocks, and mappings from route
// prefixes to handler names
ServerConfig::ServerConfig(NginxConfig config)
  : config_(config)
{
  // Store raw NginxConfig for use in getting child blocks
}

// Initializer for a ServerConfig object, Build() iterates through the passed
// in config, creating both the map from path to property values as well as
// the map from prefix to handler_name.
bool ServerConfig::Build() {
  std::vector<std::string> base_path = {};
  printPropertiesMap();
  return fillOutMaps(config_, base_path);
}

// In NgnixConfig, Statements are root level 'blocks' (e.g. server {})
// fillOutMaps will recursively construct an unordered_map between a vector of strings
// representing the 'path' to a config parameter and the value of that parameter
//   e.g. ["server", "listen"] should be mapped to the port number
bool ServerConfig::fillOutMaps(NginxConfig config, std::vector<std::string> base_path) {
  if (config.statements_.size() < 1) {
    std::cout << "Failed to build ServerConfig map, recieved empty config\n";
    return false;
  }

  // Loop through all config param block at the current depth
  for (size_t i = 0; i < config.statements_.size(); i++) {
    std::shared_ptr<NginxConfigStatement> cur_statement = config.statements_[i];
    size_t num_tokens = cur_statement->tokens_.size();
    if (num_tokens < 1) {
      std::cout << "Invalid statement: no tokens\n";
      std::cout << cur_statement->ToString(5) << std::endl;
      return false;
    }
    else if (cur_statement->child_block_ != nullptr) {
      // Dealing with an NginxConfig Block

      // If we're looking at a Route configuration block, stop recursing and
      // store the handler name in the prefix_to_handler_name_ map.
      if (num_tokens == 3 && cur_statement->tokens_[0] == "path") {
        prefix_to_handler_name_[cur_statement->tokens_[1]] = cur_statement->tokens_[2];
        continue;
      }
      else if (num_tokens == 2 && cur_statement->tokens_[0] == "default") {
        prefix_to_handler_name_["default"] = cur_statement->tokens_[1];
        continue;
      }
      else {
        std::vector<std::string> base_path_extended = base_path;
        base_path_extended.push_back(buildWordyParam(config.statements_[i], 0));
        return fillOutMaps(*(config.statements_[i]->child_block_), base_path_extended);
      }
    }
    else {
      // Dealing with an NginxConfigStatement (no child block)
      // LeafTokens denote the number of 'words' in a statement w/o a child block
      if (num_tokens < 2) {
        std::cout << "Invalid statement: no value associated with param\n";
        std::cout << cur_statement->ToString(5) << std::endl;
        return false;
      }
      size_t numLeafTokens = config.statements_[i]->tokens_.size();
      std::vector<std::string> final_path = base_path;
      final_path.push_back(buildWordyParam(config.statements_[i], 1));
      path_to_values_[final_path] = config.statements_[i]->tokens_[numLeafTokens-1];
    }
  }

  return true;
}

// The outward facing interface of ServerConfig, returns an int representing the
// status of the call and sets in *val, the value of the property passed in.
bool ServerConfig::propertyLookUp(const std::vector<std::string>& property_path,
                                  std::string& val) const {
  std::unordered_map<std::vector<std::string>,
                     std::string,
                     container_hash<std::vector<std::string>>>::const_iterator it;
  it = path_to_values_.find(property_path);

  if (it != path_to_values_.end()) {
    val = it->second;

    std::cout << "Found property '" << val << "' through path below:\n";
    for (auto const& word : property_path) {
      std::cout << word << ".";
    }
    return true;
  }
  else {
    std::cout << "\nServerConfig propertyLookUp failed with the following path:\n";
    for (auto const& word : property_path) {
      std::cout << word << ".";
    }
    std::cout << std::endl;
    return false;
  }
}

// Getter for the prefix -> handler_name unordered map
const std::unordered_map<std::string, std::string> ServerConfig::allPaths() const {
  return prefix_to_handler_name_;
}

// Returns the child NginxConfig block of the block with the uri_prefix
// passed in as input.
std::unique_ptr<NginxConfig> ServerConfig::getChildBlock(std::string uri_prefix) const {
  // Assumes that routes are specified at the top level of the config file
  for (size_t i = 0; i < config_.statements_.size(); i++) {
    std::shared_ptr<NginxConfigStatement> cur_statement = config_.statements_[i];

    // Only look at Route configuration blocks in the config
    size_t num_tokens = cur_statement->tokens_.size();
    if (num_tokens != 3 || cur_statement->tokens_[0] != "path") {
      continue;
    }

    // If the uri_prefix matches the current block's path and it has
    // a child block, return that child block
    if (cur_statement->tokens_[1] == uri_prefix &&
        cur_statement->child_block_ != nullptr) {
      return std::move(cur_statement->child_block_);
    }
  }

  std::unique_ptr<NginxConfig> empty_config =
    std::unique_ptr<NginxConfig>(new NginxConfig());
  return empty_config;
}

// Print the contents of the mapping of 'path to config param' -> value
void ServerConfig::printPropertiesMap() const {
  // Iterate over an unordered_map using range based for loop
  std::cout << "Mappings in property_to_values_:\n" << std::endl;
  for (auto element : path_to_values_) {
    for (auto const& key : element.first) {
      std::cout << key << ".";
    }
    std::cout << " -> " << element.second << std::endl;
  }

  std::cout << "Mappings in prefix_to_handler_name_:\n" << std::endl;
  for (auto element : prefix_to_handler_name_) {
    std::cout << element.first << " -> " << element.second << std::endl;
  }
}

