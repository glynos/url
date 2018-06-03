// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URL_DEFAULT_PORT_INC
#define NETWORK_DETAIL_URL_DEFAULT_PORT_INC

#include <vector>
#include <utility>
#include <string>
#include <cstdint>
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
namespace details {
using default_port_list = std::vector<std::pair<std::string, optional<std::uint16_t>>>;

const default_port_list &special_schemes();

bool is_special(string_view scheme);

optional<std::uint16_t> default_port(string_view scheme);

bool is_default_port(string_view scheme, std::uint16_t port);
}  // namespace details
}  // namespace skyr


#endif  // NETWORK_DETAIL_URL_DEFAULT_PORT_INC
