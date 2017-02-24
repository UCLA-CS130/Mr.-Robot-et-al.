#include "server_stats.h"

void ServerStats::recordInteraction(Request request, Response response) {
  // Construct tuple of [full_uri, status_code]
  std::vector<string> uri_and_status = { request.uri(), response.statusCode() };

  // Increment its count or initialize its count as 0 if never seen before
  const auto it = handler_call_distribution_.find(uri_and_status);
  if (it == handler_call_distribution_.end()) {
    handler_call_distribution_[uri_and_status] = 0;
  }
  else {
    handler_call_distribution_[uri_and_status] += 1;
  }
}

void ServerStats::recordConfig(ServerConfig server_config) {
  prefixes_to_handlers_ = server_config.allPaths();
}

std::unordered_map<string, string> ServerStats::allRoutes() const {
  return prefixes_to_handlers_;
}

std::unordered_map<std::vector<string>,
                   int,
                   tuple_hash<std::vector<string>>>
ServerStats::handlerCallDistribution() const {
  // TODO
}
