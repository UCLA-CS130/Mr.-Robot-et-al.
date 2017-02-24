#include "server_stats.h"

bool ServerStats::recordRequest(Request request) {
  // TODO: Ask for uri()
}

bool ServerStats::recordResponse(Response response) {
  // TODO: Write new statusCode() getter
}
void ServerStats::recordConfig(ServerConfig server_config) {
  prefixes_to_handlers_ = server_config.allPaths();
}

std::unordered_map<string, string> ServerStats::allRoutes() const {
  return prefixes_to_handlers_;
}

std::unordered_map<std::vector<string>,
                   int,
                   container_hash<std::vector<string>>>
ServerStats::handlerCallDistribution() const {
  // TODO
}
