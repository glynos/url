// Copyright (c) Glyn Matthews 2011-2018.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_DECODE_INC
#define SKYR_URL_DETAILS_DECODE_INC

#include <iterator>
#include <cassert>
#include <string>
#include <skyr/expected.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
namespace details {
enum class decode_errc {
  non_hex_input,
  conversion_failed,

};

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

expected<char, decode_errc> pct_decode_char(string_view input) {
  assert((input.size() >= 3) && (input.front() == '%'));

  auto it = begin(input);
  ++it;
  auto h0 = *it;
  if (h0 >= '8') {
    // unable to pct_decode characters outside the ASCII character set.
    return make_unexpected(decode_errc::conversion_failed);
  }

  auto v0 = letter_to_hex(h0);
  if (!v0) {
    return v0;
  }

  ++it;
  auto h1 = *it;
  auto v1 = letter_to_hex(h1);
  if (!v1) {
    return v1;
  }

  return static_cast<char>((0x10 * v0.value()) + v1.value());
}

template <class InputIterator, class OutputIterator>
expected<OutputIterator, decode_errc> pct_decode(
    InputIterator first, InputIterator last, OutputIterator out) {
  auto it = first;
  while (it != last) {
    if (*it == '%') {
      if (std::distance(it, last) < 3) {
        out++ = *it;
        return out;
      }
      auto c = pct_decode_char(string_view(std::addressof(*it), 3));
      if (!c) {
        return make_unexpected(std::move(c.error()));
      }
      out = c.value();
      ++out;
      it += 3;
    } else {
      *out++ = *it++;
    }
  }
  return out;
}

inline expected<std::string, decode_errc> pct_decode(string_view input) {
  auto result = std::string{};
  pct_decode(begin(input), end(input), std::back_inserter(result));
  return result;
}
}  // namespace details
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_DECODE_INC
