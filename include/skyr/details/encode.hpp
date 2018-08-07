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

namespace skyr {
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

//template <class OutputIterator>
//OutputIterator pct_encode_char(char in, OutputIterator out, const char *includes = "") {
//  if ((static_cast<std::uint32_t>(in) <= 0x1f) ||
//      (static_cast<std::uint32_t>(in) > 0x7e)) {
//    out++ = '%';
//    out++ = hex_to_letter((in >> 4) & 0x0f);
//    out++ = hex_to_letter(in & 0x0f);
//  }
//  else {
//    auto first = includes, last = includes + std::strlen(includes);
//    auto it = std::find(first, last, in);
//    if (it != last) {
//      out++ = '%';
//      out++ = hex_to_letter((in >> 4) & 0x0f);
//      out++ = hex_to_letter(in & 0x0f);
//    }
//    else {
//      out++ = in;
//    }
//  }
//  return out;
//}

std::string pct_encode_char(char in, const char *includes = "") {
  auto encoded = std::string{};
  if ((static_cast<std::uint32_t>(in) <= 0x1f) ||
      (static_cast<std::uint32_t>(in) > 0x7e)) {
    encoded += '%';
    encoded += hex_to_letter((in >> 4) & 0x0f);
    encoded += hex_to_letter(in & 0x0f);
  }
  else {
    auto first = includes, last = includes + std::strlen(includes);
    auto it = std::find(first, last, in);
    if (it != last) {
      encoded += '%';
      encoded += hex_to_letter((in >> 4) & 0x0f);
      encoded += hex_to_letter(in & 0x0f);
    }
    else {
      encoded += in;
    }
  }
  return encoded;
}

inline bool is_pct_encoded(
    string_view::const_iterator it,
    string_view::const_iterator last,
    const std::locale &locale = std::locale::classic()) {
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

inline bool is_pct_encoded(
    string_view input,
    const std::locale &locale = std::locale::classic()) {
  return is_pct_encoded(begin(input), end(input), locale);
}
}  // namespace details
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_ENCODE_INC
