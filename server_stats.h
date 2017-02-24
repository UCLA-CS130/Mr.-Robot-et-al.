#ifndef SERVER_STATS_H
#define SERVER_STATS_H

#include "request.h"
#include "response.h"
#include "server_config.h"

#include <unordered_map>
#include <vector>
#include <string>

using std::string;

class ServerStats {
  public:
    bool recordRequest(Request request);
    bool recordResponse(Response response);
    void recordConfig(ServerConfig server_config);

    // The map of prefixes to handler names
    std::unordered_map<string, string> allRoutes() const;

    // The map of [url, status_code] -> count
    std::unordered_map<std::vector<string>,
                       int,
                       container_hash<std::vector<string>>>
    handlerCallDistribution() const;

  private:

    // From: https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key
    // Allows us to use a vector as a key in an unordered_map by passing in a hash function when declaring the unordered_map.
    template <typename Container>
    struct container_hash {
      std::size_t operator()(Container const& c) const {
        return boost::hash_range(c.begin(), c.end());
      }
    };

    // uri_prefix -> handler_name
    std::unordered_map<string, string> prefixes_to_handlers_;
    // [url, status_code] -> count
    std::unordered_map<std::vector<string>,
                       int,
                       container_hash<std::vector<string>>> handler_call_distribution_;
};

#endif SERVER_STATS_H
