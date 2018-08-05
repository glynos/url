// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PARSE_INC
#define SKYR_URL_PARSE_INC

#include <string>
#include <skyr/optional.hpp>
#include <skyr/expected.hpp>
#include <skyr/url_record.hpp>
#include <skyr/url_parse_state.hpp>
#include <skyr/url_error.hpp>

namespace skyr {
/// \exclude
namespace details {
/// \param input The input string that will be parsed
/// \param base An optional base URL
/// \param url An optional `url_record`
/// \param state_override
/// \returns A `url_record` on success and an error code on failure.
expected<url_record, url_parse_errc> basic_parse(
    std::string input,
    const optional<url_record> &base = nullopt,
    const optional<url_record> &url = nullopt,
    optional<url_parse_state> state_override = nullopt);
}  // namespace details

/// Parses a URL and returns a `url_record`.
/// \param input The input string that will be parsed
/// \param base An optional base URL
/// \returns A `url_record` on success and an error code on failure.
expected<url_record, url_parse_errc> parse(
    std::string input,
    const optional<url_record> &base = nullopt);
}  // namespace skyr

#endif  // SKYR_URL_PARSE_INC
