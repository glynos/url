// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_INC
#define NETWORK_DETAIL_URI_PARSE_INC

#include <string>
#include <cstdint>
#include <array>
#include <skyr/optional.hpp>
#include <skyr/url_state.hpp>
#include <skyr/url_record.hpp>

namespace skyr {
/**
 *
 * @param input
 * @param base
 * @param url
 * @param state_override
 * @return
 */
optional<url_record> basic_parse(
    std::string input,
    const optional<url_record> &base = nullopt,
    const optional<url_record> &url = nullopt,
    optional<url_state> state_override = nullopt);

/**
 *
 * @param input
 * @param base
 * @return
 */
optional<url_record> parse(
    std::string input,
    const optional<url_record> &base = nullopt);
}  // namespace skyr

#endif  // NETWORK_DETAIL_URI_PARSE_INC
