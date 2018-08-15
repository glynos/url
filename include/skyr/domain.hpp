// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DOMAIN_HPP
#define SKYR_DOMAIN_HPP

#include <string>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
///
enum class domain_errc {
  fail=1,
  encoding_error,
  incorrect_dns_length,
};

/// \param domain
/// \param be_strict
/// returns
expected<std::string, domain_errc> domain_to_ascii(
    string_view domain,
    bool be_strict = true);

/// \param domain
/// \param be_strict
/// \returns
expected<std::string, domain_errc> domain_to_ascii(
    u32string_view domain,
    bool be_strict = true);
}  // namespace skyr

#endif //SKYR_DOMAIN_HPP
