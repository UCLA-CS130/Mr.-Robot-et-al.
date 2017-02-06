#include "request_router.h"
#include "request_handlers.h"

// TODO: Split out HTTP parsing into its own class
bool RequestRouter::routeRequest(const char* request_buffer,
                                 const size_t& request_buffer_size,
                                 char* &response_buffer,
                                 size_t& response_buffer_size) {

}
