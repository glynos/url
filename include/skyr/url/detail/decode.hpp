// Copyright (c) Glyn Matthews 2011-2018.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DECODE_INC
#define NETWORK_URI_DECODE_INC

#include <skyr/optional.hpp>
#include <skyr/url/url_errors.hpp>
#include <iterator>
#include <cassert>

namespace skyr {
namespace detail {
template <typename CharT>
optional<CharT> letter_to_hex(CharT in) {
  if ((in >= '0') && (in <= '9')) {
    return in - '0';
  }

  if ((in >= 'a') && (in <= 'f')) {
    return in + 10 - 'a';
  }

  if ((in >= 'A') && (in <= 'F')) {
    return in + 10 - 'A';
  }

  return nullopt;
}

template <class InputIterator>
optional<char> decode_char(InputIterator &it) {
  assert(*it == '%');

  ++it;
  auto h0 = *it;
  if (h0 >= '8') {
    // unable to decode characters outside the ASCII character set.
//    throw percent_decoding_error(url_error::conversion_failed);
    return nullopt;
  }

  auto v0 = detail::letter_to_hex(h0);
  if (!v0) {
//    throw percent_decoding_error(url_error::non_hex_input);
    return nullopt;
  }

  ++it;
  auto h1 = *it;
  auto v1 = detail::letter_to_hex(h1);
  if (!v1) {
//    throw percent_decoding_error(url_error::non_hex_input);
    return nullopt;
  }

  ++it;
  return static_cast<char>((0x10 * v0.value()) + v1.value());
}

template <class InputIterator, class OutputIterator>
OutputIterator decode(InputIterator in_begin, InputIterator in_end,
                      OutputIterator out_begin) {
  auto it = in_begin;
  auto out = out_begin;
  while (it != in_end) {
    if (*it == '%') {
      if (std::distance(it, in_end) < 3) {
        out++ = *it;
        return out;
//        throw percent_decoding_error(url_error::not_enough_input);
      }
//      char c = '\0';
      auto c = decode_char(it);
      if (!c) {
        throw percent_decoding_error(url_error::non_hex_input);
      }
      out = c.value();
      ++out;
    } else {
      *out++ = *it++;
    }
  }
  return out;
}

//template <class String>
//String decode(const String &source) {
//  String unencoded;
//  decode(std::begin(source), std::end(source), std::back_inserter(unencoded));
//  return unencoded;
//}
}  // namespace detail
}  // namespace skyr

#endif  // NETWORK_URI_DECODE_INC
