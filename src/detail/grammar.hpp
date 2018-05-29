// Copyright 2016-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_GRAMMAR_INC
#define NETWORK_DETAIL_URI_GRAMMAR_INC

#include <skyr/string_view.hpp>
#include <cstdlib>
#include <locale>
#include <cstring>
#include <string>

namespace skyr {
namespace detail {
inline bool is_in(string_view::value_type c,
                  string_view view) {
  return std::end(view) != std::find(std::begin(view), std::end(view), c);
}

inline bool is_in(string_view::value_type c,
                  const char *chars) {
  return is_in(c, string_view{chars});
}

inline bool is_in(string_view::const_iterator &it,
                  string_view::const_iterator last, const char *chars) {
  if (is_in(*it, chars)) {
    ++it;
    return true;
  }
  return false;
}

inline string_view sub_delims() {
  static const char sub_delims[] = {'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};
  return string_view(sub_delims, sizeof(sub_delims));
}

inline bool is_sub_delim(string_view::value_type c) {
  return is_in(c, sub_delims());
}

inline bool is_sub_delim(string_view::const_iterator &it,
                         string_view::const_iterator last) {
  if (is_sub_delim(*it)) {
    ++it;
    return true;
  }
  return false;
}

inline bool is_unreserved(string_view::value_type c) {
  return isalnum(c) || is_in(c, "-._~");
}

inline bool is_unreserved(string_view::const_iterator &it,
                          string_view::const_iterator last) {
  if (is_unreserved(*it)) {
    ++it;
    return true;
  }
  return false;
}

inline bool is_pct_encoded(string_view::const_iterator &it,
                           string_view::const_iterator last) {
  if (it == last) {
    return false;
  }

  string_view::const_iterator it_copy = it;

  if (*it_copy == '%') {
    ++it_copy;
    if (it_copy == last) {
      return false;
    }
  }

  if (std::isxdigit(*it_copy, std::locale("C"))) {
    ++it_copy;
    if (it_copy == last) {
      return false;
    }
  }

  if (std::isxdigit(*it_copy, std::locale("C"))) {
    ++it_copy;
    it = it_copy;
    return true;
  }

  return false;
}

inline bool is_valid_port(string_view::const_iterator it,
                          string_view::const_iterator last) {
  const char* port_first = std::addressof(*it);
  char* port_last = 0;
  auto value = std::strtoul(port_first, &port_last, 10);
  return (std::addressof(*last) == port_last) && (port_first != port_last) &&
         (value < std::numeric_limits<std::uint16_t>::max());
}

inline bool is_valid_port(const std::string &port) {
  auto view = string_view(port);
  return is_valid_port(std::begin(view), std::end(view));
}
}  // namespace detail
}  // namespace skyr

#endif  // NETWORK_DETAIL_URI_GRAMMAR_INC
