// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "url_schemes.hpp"

namespace network {
namespace whatwg {
namespace detail {
const std::vector<std::pair<std::string, optional<std::uint16_t>>> &special_schemes() {
  static const std::vector<std::pair<std::string, optional<std::uint16_t>>> schemes = {
    {"ftp", 21},
    {"file", nullopt},
    {"gopher", 70},
    {"http", 80},
    {"https", 443},
    {"ws", 80},
    {"wss", 443},
  };
  return schemes;
}
}  // namespace detail
}  // namespace whatwg
}  // namespace network
