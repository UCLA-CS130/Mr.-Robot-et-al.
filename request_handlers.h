#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace lightning_server {
  namespace request_handlers {
    size_t echoRequestHandler(boost::shared_ptr<tcp::socket> socket);
  } // namespace request_handlers
} // namespace lightning_server

#endif
