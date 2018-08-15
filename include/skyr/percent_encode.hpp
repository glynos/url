// Copyright (c) Glyn Matthews 2011-2018.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_ENCODE_INC
#define SKYR_URL_DETAILS_ENCODE_INC

#include <iterator>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <locale>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
///
enum class decode_errc {
  non_hex_input,
};

namespace details {
inline char hex_to_letter(char in) {
  if ((in >= 0) && (in < 10)) {
    return in + '0';
  }

  if ((in >= 10) && (in < 16)) {
    return in - char(10) + 'A';
  }

  return in;
}

inline expected<char, decode_errc> letter_to_hex(char in) {
  if ((in >= '0') && (in <= '9')) {
    return in - '0';
  }

  if ((in >= 'a') && (in <= 'f')) {
    return in + char(10) - 'a';
  }

  if ((in >= 'A') && (in <= 'F')) {
    return in + char(10) - 'A';
  }

  return make_unexpected(decode_errc::non_hex_input);
}
}  // namespace details

/// \param in
/// \param includes
/// \returns
inline std::string pct_encode_byte(char in, const char *includes = "") {
  auto encoded = std::string{};
  if ((static_cast<std::uint32_t>(in) <= 0x1f) ||
      (static_cast<std::uint32_t>(in) > 0x7e)) {
    encoded += '%';
    encoded += details::hex_to_letter((in >> 4) & 0x0f);
    encoded += details::hex_to_letter(in & 0x0f);
  }
  else {
    auto first = includes, last = includes + std::strlen(includes);
    auto it = std::find(first, last, in);
    if (it != last) {
      encoded += '%';
      encoded += details::hex_to_letter((in >> 4) & 0x0f);
      encoded += details::hex_to_letter(in & 0x0f);
    }
    else {
      encoded += in;
    }
  }
  return encoded;
}

/// \param input
/// \returns
inline expected<char, decode_errc> pct_decode_byte(string_view input) {
  if ((input.size() < 3) || (input.front() != '%')) {
    return make_unexpected(decode_errc::non_hex_input);
  }

  auto it = begin(input);
  ++it;
  auto h0 = *it;
  auto v0 = details::letter_to_hex(h0);
  if (!v0) {
    return v0;
  }

  ++it;
  auto h1 = *it;
  auto v1 = details::letter_to_hex(h1);
  if (!v1) {
    return v1;
  }

  return static_cast<char>((0x10 * v0.value()) + v1.value());
}

/// \param input
/// \returns
inline expected<std::string, decode_errc> pct_decode(string_view input) {
  auto result = std::string{};
  auto first = begin(input), last = end(input);
  auto it = first;
  while (it != last) {
    if (*it == '%') {
      if (std::distance(it, last) < 3) {
        result.push_back(*it);
        return result;
      }
      auto c = pct_decode_byte(string_view(std::addressof(*it), 3));
      if (!c) {
        return make_unexpected(std::move(c.error()));
      }
      result.push_back(c.value());
      it += 3;
    } else {
      result.push_back(*it++);
    }
  }
  return result;
}

/// \param input
/// \param locale
/// \returns
inline bool is_pct_encoded(
    string_view input,
    const std::locale &locale = std::locale::classic()) {
  auto first = begin(input), last = end(input);
  auto it = first;

  if (it == last) {
    return false;
  }

  if (*it == '%') {
    ++it;
    if (it != last) {
      if (std::isxdigit(*it, locale)) {
        ++it;
        if (it != last) {
          if (std::isxdigit(*it, locale)) {
            return true;
          }
        }
      }
    }
  }

  return false;
}
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_ENCODE_INC
