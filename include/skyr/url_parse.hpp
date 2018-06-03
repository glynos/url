// Copyright 2013-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_INC
#define NETWORK_DETAIL_URI_PARSE_INC

#include <vector>
#include <string>
#include <utility>
#include <iterator>
#include <skyr/optional.hpp>
#include <skyr/url_state.hpp>
#include <url_errors.hpp>
#include <skyr/url_record.hpp>

namespace skyr {
optional<url_record> basic_parse(
    std::string input,
    const optional<url_record> &base = nullopt,
    const optional<url_record> &url = nullopt,
    optional<url_state> state_override = nullopt);

optional<url_record> parse(
    std::string input,
    const optional<url_record> &base = nullopt);

std::string serialize(
    const url_record &url,
    bool exclude_fragment = false);
}  // namespace skyr

#endif  // NETWORK_DETAIL_URI_PARSE_INC
