#ifndef SERVER_STATS_H
#define SERVER_STATS_H

#include "request.h"
#include "response.h"
#include "server_config.h"

#include <unordered_map>
#include <vector>
#include <string>

using std::string;

// From: https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key
// Allows us to use a vector as a key in an unordered_map by passing in a hash function when declaring the unordered_map.
template <typename Container>
struct tuple_hash {
  std::size_t operator()(Container const& c) const {
    return boost::hash_range(c.begin(), c.end());
  }
};

class ServerStats {
  public:
    // Initialize distribution with one sucessful call to /status
    void init(ServerConfig server_config);

    // Record a Request/Response pair
    void recordInteraction(Request request, Response response);

    // Store the ServerConfig for later look-up
    void recordConfig(ServerConfig server_config);

    // The map of prefixes to handler names
    std::unordered_map<string, string> allRoutes() const;

    // The map of [full_uri, status_code] -> count
    std::unordered_map<std::vector<string>,
                       int,
                       tuple_hash<std::vector<string>>>
    handlerCallDistribution() const;

  private:

    // uri_prefix -> handler_name
    std::unordered_map<string, string> prefixes_to_handlers_;
    // [full_uri, status_code] -> count
    std::unordered_map<std::vector<string>,
                       int,
                       tuple_hash<std::vector<string>>> handler_call_distribution_;
};

#endif SERVER_STATS_H
