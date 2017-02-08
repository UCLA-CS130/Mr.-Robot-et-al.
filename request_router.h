#ifndef REQUEST_ROUTER_H
#define REQUEST_ROUTER_H

#include "server_config.h"
#include <cstddef>

class RequestRouter {
  public:
    // TODO: Reduce duplication between this and the handlers

    // Calls the correct request-handler
    // Returns true if request-processing succeeds, and false otherwise
    // TODO: More extensive error-handling
    bool routeRequest(const ServerConfig& server_config,
                      const char* request_buffer,
                      const size_t& request_buffer_size,
                      char* &response_buffer,
                      size_t& response_buffer_size);
};

#endif
