// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <vector>
#include "punycode.hpp"
#include "skyr/optional.hpp"

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
  return  cp - 48 < 10 ? cp - 22 :  cp - 65 < 26 ? cp - 65 :
                                    cp - 97 < 26 ? cp - 97 :  base;
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

using ucs4_convert = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;
}  // namespace

expected<std::string, punycode_errc> encode(string_view input) {
  try {
    ucs4_convert ucs4conv{};
    auto ucs4 = ucs4conv.from_bytes(begin(input), end(input));
    return encode(ucs4);
  } catch(const std::range_error&) {
    return make_unexpected(punycode_errc::bad_input);
  }
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
  }
  else {
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

  try {
    ucs4_convert ucs4conv{};
    return ucs4conv.to_bytes(result);
  } catch(const std::range_error&) {
    return make_unexpected(punycode_errc::bad_input);
  }
}
}  // namespace punycode
}  // namespace skyr
