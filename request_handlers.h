#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/asio.hpp>
namespace bai = boost::asio::ip;

namespace lightning_server {
  namespace request_handlers {
    size_t echoRequestHandler(boost::shared_ptr<bai::tcp::socket> socket);
  } // namespace request_handlers
} // namespace lightning_server

#endif
