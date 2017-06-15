// Copyright 2016-2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse.hpp"
#include <iterator>
#include <limits>
#include <arpa/inet.h>
#include <network/uri/detail/uri_parts.hpp>
#include "grammar.hpp"
#include "detail/url_schemes.hpp"

namespace network {
namespace detail {
namespace {
enum class uri_state {
  scheme,
  first_slash,
  second_slash,
  authority,
  host,
  path,
  query,
  fragment
};

bool validate_scheme(string_view::const_iterator &it,
                     string_view::const_iterator last) {
  if (it == last) {
    return false;
  }

  // The first character must be a letter
  if (!isalpha(*it)) {
    return false;
  }
  ++it;

  while (it != last) {
    if (*it == ':') {
      break;
    }
    else if (!isalnum(*it) && !is_in(*it, "+-.")) {
      return false;
    }

    ++it;
  }

  return true;
}

bool validate_user_info(string_view::const_iterator first,
                        string_view::const_iterator last) {
  if (first == last) {
    return false;
  }

  auto sep_it = std::find(first, last, ':');

  auto it = first;

  while (it != sep_it) {
    if (!is_unreserved(it, sep_it) &&
        !is_pct_encoded(it, sep_it) &&
        !is_sub_delim(it, sep_it) &&
        !is_ucschar(it, sep_it)) {
      return false;
    }
  }

  if (sep_it != last) {
    ++it;

    while (it != last) {
      if (!is_unreserved(it, last) &&
          !is_pct_encoded(it, last) &&
          !is_sub_delim(it, last) &&
          !is_ucschar(it, last)) {
        return false;
      }
    }
  }

  return true;
}

inline bool is_forbidden_host_point(string_view::value_type c) {
  static const char forbidden[] = {
    '\0', '\t', '\n', '\r', ' ', '#', '%', '/', ':', '?', '@', '[', '\\', ']',
  };
  const char *first = forbidden, *last = forbidden + sizeof(forbidden);
  return last != std::find(first, last, c);
}

bool validate_domain(string_view::const_iterator first,
                     string_view::const_iterator last) {
  auto it = first;
  while (it != last) {
    if (!is_unreserved(it, last) &&
        !is_pct_encoded(it, last) &&
        !is_sub_delim(it, last) &&
        !is_ucschar(it, last)) {
      return false;
    }
  }
  return true;
}

bool validate_ip_address(string_view::const_iterator first,
                         string_view::const_iterator last,
                         int family) {
  // the maximum ipv6 address length is 45 - choose a buffer on the
  // stack large enough
  char addr[64];
  std::memset(addr, 0, sizeof(addr));
  std::copy(first, last, std::begin(addr));
  string_view::value_type buffer[sizeof(struct in6_addr)];
  int rc = ::inet_pton(family, addr, &buffer);
  return rc > 0;
}

inline bool validate_ipv4_address(string_view::const_iterator first,
                                  string_view::const_iterator last) {
  return validate_ip_address(first, last, AF_INET);
}

inline bool validate_ipv6_address(string_view::const_iterator first,
                                  string_view::const_iterator last) {
  return validate_ip_address(++first, --last, AF_INET6);
}

bool set_host(string_view::const_iterator first,
              string_view::const_iterator last,
              uri_parts &parts) {
  auto valid_host = (last == std::find_if(first, last, is_forbidden_host_point));
  if (valid_host) {
    valid_host =
      validate_domain(first, last) ||
      validate_ipv4_address(first, last)
      ;

    if (valid_host) {
      parts.host = uri_part(first, last);
    }
  }
  return valid_host;
}

bool set_ipv6_host(string_view::const_iterator first,
                   string_view::const_iterator last,
                   uri_parts &parts) {
  if (*first != '[') {
    return false;
  }

  auto last_but_one = last;
  --last_but_one;
  if (*last_but_one != ']') {
    return false;
  }

  if (!validate_ipv6_address(first, last)) {
    return false;
  }

  parts.host = uri_part(first, last);
  return true;
}

bool set_port(string_view scheme,
              string_view::const_iterator port_first,
              string_view::const_iterator port_last,
              uri_parts &parts) {
  if (port_first != port_last) {
    if (scheme.compare("file") == 0) {
      return false;
    }

    if (!is_valid_port(port_first, port_last)) {
      return false;
    }
  }

  parts.port = uri_part(port_first, port_last);
  return true;
}

bool set_host_and_port(string_view scheme,
                       string_view::const_iterator first,
                       string_view::const_iterator last,
                       string_view::const_iterator last_colon,
                       uri_parts &parts) {
  if ((scheme.compare("file") != 0) && (first == last)) {
    return false;
  }

  if (first >= last_colon) {
    return set_host(first, last, parts);
  }
  else {
    if (*first == '[') {
      return set_ipv6_host(first, last, parts);
    }
    else {
      if (!set_host(first, last_colon, parts)) {
        return false;
      }

      auto port_first = last_colon;
      ++port_first;
      if (!set_port(scheme, port_first, last, parts)) {
        return false;
      }
    }
  }
  return true;
}

bool set_path(string_view scheme,
              string_view::const_iterator first,
              string_view::const_iterator last,
              uri_parts &parts) {
  if (first != last) {
    if ((*first == '@') ||
        (*first == ':')) {
      return false;
    }
  }

  parts.path = uri_part(first, last);
  return true;
}

bool validate_fragment(string_view::const_iterator &it,
                       string_view::const_iterator last) {
  while (it != last) {
    if (!is_pchar(it, last) && !is_in(it, last, "?/")) {
      return false;
    }
  }
  return true;
}
} // namespace

bool parse(string_view::const_iterator &it, string_view::const_iterator last,
           uri_parts &parts) {
  auto state = uri_state::scheme;

  if (it == last) {
    return false;
  }

  auto first = it;

  if (validate_scheme(it, last)) {
    parts.scheme = uri_part(first, it);

    if (detail::is_special(static_cast<string_view>(*parts.scheme))) {
      //
    }

    // move past the scheme delimiter
    ++it;
    state = uri_state::first_slash;
  }
  else {
    return false;
  }

  // Hierarchical part
  // this is used by the user_info/port
  auto last_colon = first;
  while (it != last) {
    if (state == uri_state::first_slash) {
      if (*it == '/') {
        state = uri_state::second_slash;
        // set the first iterator in case the second slash is not forthcoming
        first = it;
        ++it;
        continue;
      }
      else {
        if (detail::is_special(static_cast<string_view>(*parts.scheme))) {
          return false;
        }

        state = uri_state::path;
        first = it;
      }
    }
    else if (state == uri_state::second_slash) {
      if (*it == '/') {
        state = uri_state::authority;
        ++it;
        first = it;
        continue;
      }
      else {
        // it's a valid URI, and this is the beginning of the path
        // state = uri_state::path;
        return false;
      }
    }
    else if (state == uri_state::authority) {
      // reset the last colon
      if (first == it) {
        last_colon = first;
      }

      if (*it == '@') {
        if (!validate_user_info(first, it)) {
          return false;
        }
        parts.user_info = uri_part(first, it);
        state = uri_state::host;
        ++it;
        first = it;
        continue;
      }
      else if (*it == ':') {
        last_colon = it;
      }
      else if (*it == '/') {
        // we skipped right past the host and port, and are at the path.
        if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first, it, last_colon, parts)) {
          return false;
        }
        state = uri_state::path;
        first = it;
        continue;
      }
      else if (*it == '?') {
        // the path is empty, but valid, and the next part is the query
        if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first, it, last_colon, parts)) {
          return false;
        }

        if (!set_path(static_cast<string_view>(*parts.scheme), it, it, parts)) {
          return false;
        }

        state = uri_state::query;
        ++it;
        first = it;
        break;
      }
      else if (*it == '#') {
        // the path is empty, but valid, and the next part is the fragment
        if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first, it, last_colon, parts)) {
          return false;
        }

        if (!set_path(static_cast<string_view>(*parts.scheme), it, it, parts)) {
          return false;
        }

        state = uri_state::fragment;
        ++it;
        first = it;
        break;
      }
    }
    else if (state == uri_state::host) {
      if (*first == ':') {
        return false;
      }

      if (*it == ':') {
        last_colon = it;
      }
      else if (*it == '/') {
        if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first, it, last_colon, parts)) {
          return false;
        }

        state = uri_state::path;
        first = it;
        continue;
      }
      else if (*it == '?') {
        // the path is empty, but valid, and the next part is the query
        if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first, it, last_colon, parts)) {
          return false;
        }

        if (!set_path(static_cast<string_view>(*parts.scheme), it, it, parts)) {
          return false;
        }

        state = uri_state::query;
        ++it;
        first = it;
        break;
      }
      else if (*it == '#') {
        // the path is empty, but valid, and the next part is the fragment
        if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first, it, last_colon, parts)) {
          return false;
        }

        if (!set_path(static_cast<string_view>(*parts.scheme), it, it, parts)) {
          return false;
        }

        state = uri_state::fragment;
        ++it;
        first = it;
        break;
      }
    }
    else if (state == uri_state::path) {
      if (*it == '?') {
        if (!set_path(static_cast<string_view>(*parts.scheme), first, it, parts)) {
          return false;
        }

        // move past the query delimiter
        ++it;
        first = it;
        state = uri_state::query;
        break;
      }
      else if (*it == '#') {
        if (!set_path(static_cast<string_view>(*parts.scheme), first, it, parts)) {
          return false;
        }

        // move past the fragment delimiter
        ++it;
        first = it;
        state = uri_state::fragment;
        break;
      }

      if (!is_pchar(it, last) && !is_in(it, last, "/")) {
        return false;
      }
      else {
        continue;
      }
    }

    ++it;
  }

  if (state == uri_state::query) {
    while (it != last) {
      if (!is_pchar(it, last) && !is_in(it, last, "?/")) {
        // If this is a fragment, keep going
        if (*it == '#') {
          parts.query = uri_part(first, it);
          // move past the fragment delimiter
          ++it;
          first = it;
          state = uri_state::fragment;
          break;
        }
        else {
          return false;
        }
      }
    }
  }

  if (state == uri_state::fragment) {
    if (!validate_fragment(it, last)) {
      return false;
    }
  }

  // we're done!
  if ((state == uri_state::first_slash) ||
      (state == uri_state::second_slash)) {
    return false;
  }
  else if (state == uri_state::authority) {
    if (first == last) {
      return false;
    }

    if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first,
                           last, last_colon, parts)) {
      return false;
    }

    if (!set_path(static_cast<string_view>(*parts.scheme), last, last,
                  parts)) {
      return false;
    }
  }
  else if (state == uri_state::host) {
    if (first == last) {
      return false;
    }

    if (!set_host_and_port(static_cast<string_view>(*parts.scheme), first,
                           last, last_colon, parts)) {
      return false;
    }

    if (!set_path(static_cast<string_view>(*parts.scheme), last, last,
                  parts)) {
      return false;
    }
  }
  else if (state == uri_state::path) {
    if (!set_path(static_cast<string_view>(*parts.scheme), first, last,
                  parts)) {
      return false;
    }
  }
  else if (state == uri_state::query) {
    parts.query = uri_part(first, last);
  }
  else if (state == uri_state::fragment) {
    parts.fragment = uri_part(first, last);
  }

  return true;
}
}  // namespace detail
}  // namespace network
