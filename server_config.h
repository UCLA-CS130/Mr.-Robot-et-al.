#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "config_parser.h"
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <vector>

// From: https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key
// Allows us to use a vector as a key in an unordered_map by passing in a hash function when declaring the unordered_map.
template <typename Container>
struct container_hash {
  std::size_t operator()(Container const& c) const {
    return boost::hash_range(c.begin(), c.end());
  }
};

class ServerConfig {
  public:
    ServerConfig(NginxConfig config);
    ~ServerConfig();
    int propertyLookUp(const std::vector<std::string>& propertyPath, std::string* val);

  private:
    void printPropertiesMap();
    void fillOutMap(NginxConfig config, std::vector<std::string>);
    std::unordered_map<std::vector<std::string>,
                       std::string,
                       container_hash<std::vector<std::string>>> path_to_values_;
};

#endif
