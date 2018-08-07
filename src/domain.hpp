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
namespace details {
enum class domain_errc {
  fail=1,
};

expected<std::string, domain_errc> process(
    string_view domain_name,
    bool use_std3_ascii_rules,
    bool check_hyphens,
    bool check_bidi,
    bool check_joiners,
    bool transitional_processing);

expected<std::string, domain_errc> unicode_to_ascii(
    string_view domain_name,
    bool check_hyphens,
    bool check_bidi,
    bool check_joiners,
    bool use_std3_ascii_rules,
    bool transitional_processing,
    bool verify_dns_length);
}  // namespace details
}  // namespace skyr

#endif //SKYR_DOMAIN_HPP
