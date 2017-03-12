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
    ServerConfig();
    ServerConfig(NginxConfig config);
    bool build();
    bool propertyLookUp(const std::vector<std::string>& property_path,
                        std::string& val) const;
    const std::unordered_map<std::string, std::string> allPaths() const;
    std::unique_ptr<NginxConfig> getChildBlock(std::string uri_prefix) const;
    void printPropertiesMap() const;

  private:
    bool fillOutMaps(NginxConfig config, std::vector<std::string> base_path);
    NginxConfig config_;
    std::unordered_map<std::vector<std::string>,
                       std::string,
                       container_hash<std::vector<std::string>>> path_to_values_;
    std::unordered_map<std::string,
                       std::string> prefix_to_handler_name_;
};

#endif
