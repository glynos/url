// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <locale>
#include "domain.hpp"

namespace skyr {
namespace details {
namespace {
enum class idna_status {
  disallowed,
  ignored,
  mapped,
  deviation,
  valid,
};

idna_status map_status(char c) {
  return idna_status::mapped;
}

char map(char c) {
  return std::tolower(c, std::locale::classic());
}
}  // namespace

expected<std::string, domain_errc> process(
    string_view domain_name,
    bool use_std3_ascii_rules,
    bool check_hyphens,
    bool check_bidi,
    bool check_joiners,
    bool transitional_processing) {
  auto result = std::string();
  auto error = false;

  auto first = begin(domain_name), last = end(domain_name);
  auto it = first;

  while (it != last) {
    switch (map_status(*it)) {
      case idna_status::disallowed:
        error = true;
        break;
      case idna_status::ignored:
        break;
      case idna_status::mapped:
        result += map(*it);
        break;
      case idna_status::deviation:
        if (transitional_processing) {
          result += map(*it);
        }
        else {
          result += *it;
        }
        break;
      case idna_status::valid:
        result += *it;
        break;
    }

    ++it;
  }

  if (error) {
    return make_unexpected(domain_errc::fail);
  }

  // TODO: normalize

  // TODO: break
//  auto labels = std::vector<std::string>{};

  // TODO: convert
//  for (auto &&label : labels) {
////    if (starts_with(label), "xn--")
////    else
//  }

  return result;
}

expected<std::string, domain_errc> unicode_to_ascii(
    string_view domain_name,
    bool check_hyphens,
    bool check_bidi,
    bool check_joiners,
    bool use_std3_ascii_rules,
    bool transitional_processing,
    bool verify_dns_length) {
  auto ascii_domain = process(
      domain_name,
      use_std3_ascii_rules,
      check_hyphens,
      check_bidi,
      check_joiners,
      transitional_processing);

  if (verify_dns_length) {

  }

  return ascii_domain;
}

expected<std::string, domain_errc> domain_to_ascii(
    string_view domain,
    bool be_strict) {
  auto result = unicode_to_ascii(
      domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    // validation error
  }
  return result;
}
}  // namespace details
}  // namespace skyr
