// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <algorithm>
#include <vector>
#include "skyr/unicode.hpp"
#include "skyr/domain.hpp"

namespace skyr {
namespace punycode {
namespace {
static const char32_t base = 36;
static const char32_t tmin = 1;
static const char32_t tmax = 26;
static const char32_t skew = 38;
static const char32_t damp = 700;
static const char32_t initial_bias = 72;
static const char32_t initial_n = 0x80;
static const char32_t delimiter = 0x2D;

static char32_t decode_digit(char32_t cp) {
  return cp - 48 < 10 ? cp - 22 : cp - 65 < 26 ? cp - 65 :
                                  cp - 97 < 26 ? cp - 97 : base;
}

static char encode_digit(char32_t d, int flag) {
  return d + 22 + 75 * (d < 26) - ((flag != 0) << 5);
}

static char32_t adapt(
    char32_t delta, char32_t numpoints, bool firsttime) {
  delta = firsttime ? delta / damp : delta >> 1;
  delta += delta / numpoints;

  auto k = char32_t(0);
  while (delta > ((base - tmin) * tmax) / 2) {
    delta /= base - tmin;
    k += base;
  }
  return k + (base - tmin + 1) * delta / (delta + skew);
}

std::string to_ascii(std::string input) {
  return "xn--" + input;
}

inline bool delim(char32_t c) {
  return c == delimiter;
}
}  // namespace

expected<std::string, punycode_errc> encode(string_view input) {
  auto ucs4 = ucs4_from_bytes(input);
  if (!ucs4) {
    return make_unexpected(punycode_errc::bad_input);
  }
  return encode(ucs4.value());
}

expected<std::string, punycode_errc> encode(u32string_view input) {
  auto result = std::string{};
  result.reserve(256);

  auto n = initial_n;
  auto delta = 0U;
  auto bias = initial_bias;

  for (auto c : input) {
    if (c < 0x80) {
      result += c;
    }
  }

  auto h = result.size();
  auto b = result.size();

  if (b > 0) {
    result += delimiter;
  }

  while (h < input.size()) {
    auto m = std::numeric_limits<char32_t>::max();
    for (auto c : input) {
      if ((c >= n) && (c < m)) {
        m = c;
      }
    }

    if ((m - n) > ((std::numeric_limits<char32_t>::max() - delta) / (h + 1))) {
      return make_unexpected(punycode_errc::overflow);
    }
    delta += (m - n) * (h + 1);
    n = m;

    for (auto c : input) {
      if (c < n) {
        if (++delta == 0) {
          return make_unexpected(punycode_errc::overflow);
        }
      }

      if (c == n) {
        auto q = delta;
        auto k = uint32_t(base);
        while (true) {
          auto t = k <= bias ? tmin :
                   k >= bias + tmax ? tmax : k - bias;
          if (q < t) {
            break;
          }
          result += encode_digit(t + (q - t) % (base - t), 0);
          q = (q - t) / (base - t);
          k += base;
        }

        result += encode_digit(q, 0);
        bias = adapt(delta, (h + 1), (h == b));
        delta = 0;
        ++h;
      }
    }

    ++delta, ++n;
  }

  return to_ascii(result);
}

expected<std::string, punycode_errc> decode(string_view input) {
  auto result = std::u32string();
  result.reserve(256);

  if (input.substr(0, 4).compare("xn--") == 0) {
    input.remove_prefix(4);
  } else {
    return input.to_string();
  }

  auto n = initial_n;
  auto bias = initial_bias;

  auto basic = 0U;
  for (auto j = 0U; j < input.size(); ++j) {
    if (delim(input[j])) {
      basic = j;
    }
  }

  for (auto j = 0U; j < basic; ++j) {
    result += input[j];
  }

  auto in = (basic > 0U) ? (basic + 1U) : 0U;
  auto i = 0U;
  while (in < input.size()) {
    auto oldi = i;

    auto w = 1U;
    auto k = base;
    while (true) {
      if (in >= input.size()) {
        return make_unexpected(punycode_errc::bad_input);
      }
      auto digit = decode_digit(input[in++]);
      if (digit >= base) {
        return make_unexpected(punycode_errc::bad_input);
      }
      if (digit > ((std::numeric_limits<char32_t>::max() - i) / w)) {
        return make_unexpected(punycode_errc::overflow);
      }
      i += digit * w;
      auto t = (k <= bias) ? tmin :
               ((k >= (bias + tmax)) ? tmax : (k - bias));
      if (digit < t) {
        break;
      }
      if (w > (std::numeric_limits<char32_t>::max() / (base - t))) {
        return make_unexpected(punycode_errc::overflow);
      }
      w *= (base - t);
      k += base;
    }

    auto out = result.size() + 1U;
    bias = adapt((i - oldi), out, (oldi == 0U));

    if ((i / out) > (std::numeric_limits<char32_t>::max() - n)) {
      return make_unexpected(punycode_errc::overflow);
    }
    n += i / out;
    i %= out;

    result.insert(i++, 1, n);
  }

  auto bytes = ucs4_to_bytes(result);
  if (!bytes) {
    return make_unexpected(punycode_errc::bad_input);
  }
  return bytes.value();
}
}  // namespace punycode

namespace {
enum class idna_status {
  disallowed,
  ignored,
  mapped,
  deviation,
  valid,
};

idna_status map_status(char32_t c, bool use_std3_ascii_rules) {
  if ((c == 0x00ad) ||
      (c == 0x034f) ||
      ((c >= 0x180b) && (c <= 0x180d)) ||
      (c == 0x200b) ||
      (c == 0x2060) ||
      (c == 0x2064) ||
      ((c >= 0xfe00) && (c <= 0xfe0f)) ||
      (c == 0xfeff) ||
      ((c >= 0x1bca0) && (c <= 0x1bca3)) ||
      ((c >= 0xe0100) && (c <= 0xe01ef))) {
     return idna_status::ignored;
  }
  else if (
      ((c >= 0x0041) && (c <= 0x005a)) ||
      (c == 0x3002) ||
      ((c >= 0xff0d) && (c <= 0xff0e)) ||
      ((c >= 0xff10) && (c <= 0xff19)) ||
      ((c >= 0xff21) && (c <= 0xff3a)) ||
      ((c >= 0xff41) && (c <= 0xff5a)) ||
      ((c >= 0xff5f) && (c <= 0xff60))) {
    return idna_status::mapped;
  }
  else if (
      (c == 0x00a0) ||
      (c == 0x3000) ||
      ((c >= 0xfdd0) && (c <= 0xfdef)) ||
      ((c >= 0xff00) && (c <= 0xff0c)) ||
      ((c >= 0xffef) && (c <= 0xffff))) {
    return idna_status::disallowed;
  }

  return idna_status::valid;
}

char32_t map(char32_t c) {
  if ((c >= 0x0041) && (c <= 0x005a)) {
    // upper to lower case
    return c + 0x0020;
  }
  else if (c == 0x3002) {
    return 0x002e;
  }
  else if (
      ((c >= 0xff01) && (c <= 0xff20)) ||
      ((c >= 0xff3b) && (c <= 0xff5e))) {
    // fullwidth characters
    return c - 0xfee0;
  }
  else if ((c >= 0xff21) && (0xff3a)) {
    // fullwidth characters to lower case
    return c - 0xfec0;
  }
  else if (
      ((c >= 0xff5f) && (c <= 0xff60))) {
    // fullwidth characters
    return c - 0xfeff;
  }
  return c;
}

expected<std::u32string, domain_errc> process(
    u32string_view domain_name,
    bool use_std3_ascii_rules,
    bool check_hyphens,
    bool check_bidi,
    bool check_joiners,
    bool transitional_processing) {
  auto result = std::u32string();
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

bool is_ascii(u32string_view input) noexcept {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(
      first, last,
      [] (char32_t c) -> bool {
        return c > 0x007e;
      });
  return it == last;
}

std::vector<std::u32string> split(u32string_view domain) noexcept {
  auto labels = std::vector<std::u32string>{};
  if (!domain.empty()) {
    auto first = begin(domain), last = end(domain);
    auto it = first;
    auto prev = it;
    while (it != last) {
      if (*it == '.') {
        labels.emplace_back(prev, it);
        ++it;
        prev = it;
      } else {
        ++it;
      }
    }
    labels.emplace_back(prev, it);
  }
  return labels;
}

std::u32string join(const std::vector<std::u32string> &labels) {
  auto domain = std::u32string();
  for (const auto &label : labels) {
    domain += label + U".";
  }
  return domain.substr(0, domain.length() - 1);
}

expected<std::string, domain_errc> unicode_to_ascii(
    u32string_view domain_name,
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
      label.assign(begin(encoded.value()), end(encoded.value()));
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

  auto ucs4_domain = join(labels);
  auto ascii_domain = ucs4_to_bytes(ucs4_domain);
  if (!ascii_domain) {
    return make_unexpected(domain_errc::encoding_error);
  }
  return ascii_domain.value();
}
}  // namespace

expected<std::string, domain_errc> domain_to_ascii(
    string_view domain,
    bool be_strict) {
  auto ucs4 = ucs4_from_bytes(domain);
  if (!ucs4) {
    return make_unexpected(domain_errc::encoding_error);
  }
  return domain_to_ascii(ucs4.value(), be_strict);
}

expected<std::string, domain_errc> domain_to_ascii(
    u32string_view domain,
    bool be_strict) {
  auto result = unicode_to_ascii(
      domain, false, true, true, be_strict, false, be_strict);
  if (!result) {
    // validation error
    return make_unexpected(std::move(result.error()));
  }
  return result;
}
}  // namespace skyr
