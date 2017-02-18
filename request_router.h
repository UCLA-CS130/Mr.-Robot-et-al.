#ifndef REQUEST_ROUTER_H
#define REQUEST_ROUTER_H

#include "request_handlers.h"
#include "server_config.h"

#include <memory>
#include <map>

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
    // Notice the lack of trailing slash: /prefix, not /prefix/
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
    std::unique_ptr<RequestHandler> routeRequest(const std::string& full_uri) const;

  private:
    // Map URI prefixes to request handlers
    // TODO: Creating a unique_ptr to a polymorphic type in an <unordered_map>
    // requires a specialization of std::hash. We can use <map> for now
    // See: https://stackoverflow.com/questions/20965200/stdhash-for-unique-ptr-in-unordered-map
    std::map<const std::string, const std::unique_ptr<RequestHandler>>
      handlers_map_;
};

#endif
