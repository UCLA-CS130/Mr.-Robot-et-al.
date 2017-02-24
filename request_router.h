#ifndef REQUEST_ROUTER_H
#define REQUEST_ROUTER_H

#include "request_handlers.h"
#include "server_config.h"

#include <memory>
#include <map>

class ServerStats;

// Stores and allows access to the mapping of (uri_prefix -> RequestHandler),
// which we call a "route".
//
// Also initializes and stores our long-lived RequestHandler instances
class RequestRouter {
  public:
    RequestRouter(ServerStats* server_stats);
    ~RequestRouter();

    // Given a ServerConfig, initialize RequestHandler's and map URI prefixes
    // to them. Prefixes are assumed to be unique within the config file
    //
    // Returns true if all routes are built successfully and false otherwise
    // (e.g., a non-existent handler is named in the config file and fails to be built)
    //
    // Example: path /static1 StaticHandler { ... }
    //          path /static1/subaction DynamicServeHandler { ... }
    //
    // would create the mapping:
    //          /static1 -> StaticHandler*
    //          /static1/subaction -> DynamicServeHandler*
    //
    // Notice the lack of trailing slash: /prefix, not /prefix/
    bool buildRoutes(const ServerConfig& server_config);

    // Map full URIs to the appropriate RequestHandler,
    // with longest-prefix matching for when URIs overlap.
    // Returns 404 handler when no URI prefix matches
    //
    // Example: given the above config file, a request with full_uri:
    //
    //          /static1/subaction/subdir/file1.txt
    //
    // would give back DynamicServeHandler, not StaticHandler,
    // as /static1/subaction is the longest prefix that matches.
    RequestHandler* routeRequest(const std::string& full_uri) const;

  private:
    // Map URI prefixes to request handlers
    // TODO: Creating a unique_ptr to a polymorphic type in an <unordered_map>
    // requires a specialization of std::hash. We can use <map> for now
    // See: https://stackoverflow.com/questions/20965200/stdhash-for-unique-ptr-in-unordered-map
    std::map<const std::string, RequestHandler*> handlers_map_;

    RequestHandler* not_found_handler_;
    ServerStats* server_stats_;
};

#endif
