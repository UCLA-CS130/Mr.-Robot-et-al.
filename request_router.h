#ifndef REQUEST_ROUTER_H
#define REQUEST_ROUTER_H

#include "request_handlers.h"
#include "server_config.h"

#include <memory>

// Stores the mapping of (uri_prefix -> RequestHandler),
// which we call a "route".
//
// Also initializes RequestHandlers with their URI prefix
// and config block-specific options.
class RequestRouter {
  public:
    // Given a ServerConfig, initialize RequestHandler's and map URI prefixes
    // to them. Prefixes are assumed to be unique within the config file
    //
    // Example: path /static1 StaticHandler { ... }
    //          path /static1/subaction DynamicServeHandler { ... }
    //
    // would create the mapping:
    //          /static1 -> StaticHandler
    //          /static1/subaction -> DynamicServeHandler
    //
    // And each handler would store their uri_prefix + the NginxConfig
    // child-block corresponding to that path.
    RequestRouter(const ServerConfig& server_config);

    // Map full URIs to the appropriate RequestHandler,
    // with longest-prefix matching for when URIs overlap.
    // Returns 404 handler when no URI prefix matches
    //
    // Example: given the above config file, a request with full_uri:
    //
    //          /static1/subaction/subdir/file1.txt
    //
    // would give back DynamicServeHandler, not StaticHandler,
    // as /static1/subaction is the longest-prefix that matches.
    std::unique_ptr<RequestHandler> routeRequest(const std::string& full_uri);
};

#endif
