// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <locale>
#include "domain.hpp"
#include "punycode.hpp"

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

  return result;
}

namespace {
bool is_ascii(string_view input) noexcept {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(
      first, last,
      [] (char c) -> bool {
        return !std::isprint(c, std::locale::classic());
      });
  return it == last;
}

std::vector<std::string> split(string_view domain) noexcept {
  auto labels = std::vector<std::string>{};
  auto first = begin(domain), last = end(domain);
  auto it = first;
  auto prev = it;
  while (it != last) {
    if (*it == '.') {
      labels.emplace_back(prev, it);
      ++it;
      prev = it;
    }
    else {
      ++it;
    }
  }
  labels.emplace_back(prev, it);
  return labels;
}

std::string join(const std::vector<std::string> &labels) {
  auto domain = std::string();
  for (const auto &label : labels) {
    domain += label + ".";
  }
  return domain.substr(0, domain.length() - 1);
}
}  // namespace

expected<std::string, domain_errc> unicode_to_ascii(
    string_view domain_name,
    bool check_hyphens,
    bool check_bidi,
    bool check_joiners,
    bool use_std3_ascii_rules,
    bool transitional_processing,
    bool verify_dns_length) {
  auto domain = process(
      domain_name,
      use_std3_ascii_rules,
      check_hyphens,
      check_bidi,
      check_joiners,
      transitional_processing);

  if (!domain) {
    return make_unexpected(std::move(domain.error()));
  }

  auto labels = split(domain.value());

  for (auto &label : labels) {
    if (!is_ascii(label)) {
      auto encoded = punycode::encode(label);
      if (!encoded) {
        return make_unexpected(domain_errc::encoding_error);
      }
      label = encoded.value();
    }
  }

  if (verify_dns_length) {
//    auto length = domain.value().size();
//    if ((length < 1) || (length > 253)) {
//      return make_unexpected(domain_errc::incorrect_length);
//    }
//
//    for (const auto &label : labels) {
//      auto label_length = label.size();
//      if ((label_length < 1) || (label_length > 63)) {
//        return make_unexpected(domain_errc::incorrect_length);
//      }
//    }
  }

  return join(labels);
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
