#include "request_handlers.h"
#include "request_router.h"

bool RequestRouter::buildRoutes(const ServerConfig& server_config) {
  // Iterate over uri_prefix -> handler_name map
  for (const auto path : server_config.allPaths()) {
    const std::string uri_prefix = path.first;
    const std::string handler_name = path.second;

    std::unique_ptr<RequestHandler> handler_instance = RequestHandler::CreateByName(handler_name);
    if (handler_instance == nullptr) {
      return false;
    }
    else {
      if (! handler_instance->init(uri_prefix, *(server_config.getChildBlock(uri_prefix)))) {
        return false;
      }

      handlers_map_[uri_prefix] = std::move(handler_instance);
    }
  }

  // Build special 404 route, which is our fall-through error case
  not_found_handler_ = RequestHandler::CreateByName("NotFoundHandler");
  return true;
}

std::unique_ptr<RequestHandler> RequestRouter::routeRequest(const std::string& full_uri) const {
  // Starting at the end of the string,
  // try longest-prefix match up to latest forward-slash
  //
  // Example: for the full_uri '/echo/subaction/subdir/file.txt',
  // we try '/echo/subaction/subdir', then '/echo/subaction',
  // and finally '/echo'.
  //
  // Note the lack of trailing slashes
  // See also: http://www.cplusplus.com/reference/string/string/find_first_of/
  for (int search_index = full_uri.size() - 1; search_index >= 0; search_index--) {
    size_t pos = full_uri.find_first_of('/', search_index);
    if (pos == std::string::npos) {
      continue;
    }
    else {
      // Slice string from 0 to pos; substr takes start and length
      std::string possible_prefix = full_uri.substr(0, pos);
      std::cerr << "RequestRouter::routeRequest prefix substring" << possible_prefix << std::endl;

      const auto it = handlers_map_.find(possible_prefix);
      std::cout << "@@@@@@@@@@@@@@@@@@@@Handler's map find iterator" << it->first << std::endl;
      if (it == handlers_map_.end()) {
        continue;
      }
      else {
        // Found the matching longest-prefix; return corresponding handler
        return it->second;
      }
    }
  }
  // If no prefix found, return the default handler
  // (a default handler must always be specified in the config)
  return handlers_map_.find("default")->second;
}
