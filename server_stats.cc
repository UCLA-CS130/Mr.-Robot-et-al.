#include "server_stats.h"

bool ServerStats::recordRequest(Request request) {
  // TODO
}

bool ServerStats::recordResponse(Response response) {
  // TODO
}
void ServerStats::recordConfig(ServerConfig server_config) {
  // TODO
}

std::unordered_map<string, string> ServerStats::allRoutes() const {
  // TODO
}

std::unordered_map<std::vector<string>,
                   int,
                   container_hash<std::vector<string>>>
ServerStats::handlerCallDistribution() const {
  // TODO
}
