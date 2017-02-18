#include "request_handlers.h"
#include "request_router.h"

RequestRouter::RequestRouter(const ServerConfig& server_config) {
  // TODO
}

std::unique_ptr<RequestHandler> RequestRouter::routeRequest(const std::string& full_uri) {
  // TODO
}