// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URL_DEFAULT_PORT_INC
#define NETWORK_DETAIL_URL_DEFAULT_PORT_INC

#include <vector>
#include <utility>
#include <string>
#include <cstdint>
#include <network/optional.hpp>

namespace network {
namespace whatwg {
namespace detail {
const std::vector<std::pair<std::string, optional<std::uint16_t>>>
    &special_schemes();
}  // namespace detail
}  // namespace whatwg
}  // namespace network


#endif  // NETWORK_DETAIL_URL_DEFAULT_PORT_INC
