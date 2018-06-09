// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_SERIALIZE_INC
#define NETWORK_DETAIL_URI_SERIALIZE_INC

#include <string>
#include <skyr/url_record.hpp>

namespace skyr {
/**
 *
 * @param url
 * @param exclude_fragment
 * @return
 */
std::string serialize(
    const url_record &url,
    bool exclude_fragment = false);
}  // namespace skyr

#endif  // NETWORK_DETAIL_URI_PARSE_INC
