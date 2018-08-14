// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <codecvt>
#include <locale>
#include "skyr/punycode.hpp"
#include "skyr/domain.hpp"

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

idna_status map_status(char32_t c, bool use_std3_ascii_rules) {
//  if ((c == 0x00ad) ||
//      (c == 0x034f) ||
//      ((c >= 0x180b) && (c <= 0x180d)) ||
//      (c == 0x200b) ||
//      (c == 0x2060) ||
//      (c == 0x2064) ||
//      ((c >= 0xfe00) && (c <= 0xfe0f)) ||
//      (c == 0xfeff) ||
//      ((c >= 0x1bca0) && (c <= 0x1bca3)) ||
//      ((c >= 0xe0100) && (c <= 0xe01ef))) {
//    // return idna_status::ignored;
//  }
//  else if (
//      ((c >= 0x0041) && (c <= 0x005a)) ||
//      (c == 0x3002) ||
//      ((c >= 0xff0d) && (c <= 0xff0e)) ||
//      ((c >= 0xff10) && (c <= 0xff19)) ||
//      ((c >= 0xff21) && (c <= 0xff3a)) ||
//      ((c >= 0xff41) && (c <= 0xff5a)) ||
//      ((c >= 0xff5f) && (c <= 0xff60))) {
//    return idna_status::mapped;
//  }
//
//  return idna_status::valid;
  return idna_status::mapped;
}

char32_t map(char32_t c) {
  if ((c >= 0x0041) && (c <= 0x005a)) {
    // upper to lower case
    return c + 0x0020;
  }
//  else if (c == 0x3002) {
//    return 0x002e;
//  }
//  else if (
//      ((c >= 0xff01) && (c <= 0xff5e))) {
//    // fullwidth characters
//    return c - 0xfee0;
//  }
//  else if (
//      ((c >= 0xff5f) && (c <= 0xff60))) {
//    // fullwidth characters
//    return c - 0xfeff;
//  }
  return c;
}
//
//using ucs4_convert = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;
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
    switch (map_status(*it, use_std3_ascii_rules)) {
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
  try {
//    ucs4_convert ucs4conv{};
//    auto ucs4 = ucs4conv.from_bytes(begin(domain_name), end(domain_name));
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
  } catch(const std::range_error&) {
    return make_unexpected(domain_errc::fail);
  }
}
}  // namespace details

expected<std::string, domain_errc> domain_to_ascii(
    string_view domain,
    bool be_strict) {
  auto result = details::unicode_to_ascii(
      domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    // validation error
  }
  return result;
}
}  // namespace skyr
