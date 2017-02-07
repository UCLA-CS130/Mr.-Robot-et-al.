//
// mime_types.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Cite: http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/
//     example/http/server/mime_types.h

#ifndef HTTP_MIME_TYPES_H
#define HTTP_MIME_TYPES_H

#include <string>

namespace mime_types {
  // Convert a file extension into a MIME type.
  std::string extension_to_type(const std::string& extension);
} // namespace mime_types

#endif // HTTP_MIME_TYPES_H
