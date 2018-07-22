// Copyright (c) Glyn Matthews 2011-2018.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_DECODE_INC
#define SKYR_URL_DETAILS_DECODE_INC

#include <skyr/optional.hpp>
#include <iterator>
#include <cassert>

namespace skyr {
namespace details {
inline optional<char> letter_to_hex(char in) {
  if ((in >= '0') && (in <= '9')) {
    return in - '0';
  }

  if ((in >= 'a') && (in <= 'f')) {
    return in + char(10) - 'a';
  }

  if ((in >= 'A') && (in <= 'F')) {
    return in + char(10) - 'A';
  }

  return nullopt;
}

template <class InputIterator>
optional<char> pct_decode_char(InputIterator &it) {
  assert(*it == '%');

  ++it;
  auto h0 = *it;
  if (h0 >= '8') {
    // unable to pct_decode characters outside the ASCII character set.
    return nullopt;
  }

  auto v0 = letter_to_hex(h0);
  if (!v0) {
    return nullopt;
  }

  ++it;
  auto h1 = *it;
  auto v1 = letter_to_hex(h1);
  if (!v1) {
    return nullopt;
  }

  ++it;
  return static_cast<char>((0x10 * v0.value()) + v1.value());
}

template <class InputIterator, class OutputIterator>
OutputIterator pct_decode(InputIterator first, InputIterator last,
                          OutputIterator out) {
  auto it = first;
  while (it != last) {
    if (*it == '%') {
      if (std::distance(it, last) < 3) {
        out++ = *it;
        return out;
      }
      auto c = pct_decode_char(it);
      if (!c) {
        throw std::runtime_error("Invalid character");
      }
      out = c.value();
      ++out;
    } else {
      *out++ = *it++;
    }
  }
  return out;
}
}  // namespace details
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_DECODE_INC
