#include "request_handlers.h"
#include "request_router.h"

typedef std::map<const std::string, const std::unique_ptr<RequestHandler>> HandlersMap;

bool RequestRouter::BuildRoutes(const ServerConfig& server_config) {
  // TODO: Build prefix -> RequestHandler* map

  // TODO: Build special 404 route (perhaps as internal member)

  // TODO: Init() each new RequestHandler instance

  return false;
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
      // TODO: off-by-one?
      std::string possible_prefix = full_uri.substr(0, full_uri.size() - pos);
      std::cerr << "RequestRouter::routeRequest prefix substring" << possible_prefix << std::endl;

      HandlersMap::const_iterator it = handlers_map_.find(possible_prefix);
      if (it == handlers_map_.end()) {
        continue;
      }
      else {
        // Found the matching longest-prefix; return corresponding handler
        return it->second;
      }
    }
  }
  // TODO: If no prefix found, return 404 handler
}