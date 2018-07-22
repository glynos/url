// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SERIALIZE_INC
#define SKYR_URL_SERIALIZE_INC

#include <string>
#include <skyr/url_record.hpp>

namespace skyr {
/// \param url A URL record.
/// \param exclude_fragment Indicates whether to exclude the URL fragment or not.
/// \returns A serialize URL string.
std::string serialize(
    const url_record &url,
    bool exclude_fragment = false);
}  // namespace skyr

#endif  // SKYR_URL_SERIALIZE_INC
