// Copyright 2016-2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse.hpp"
#include <iterator>
#include <limits>
#include <arpa/inet.h>
#include "grammar.hpp"
#include <network/uri/detail/uri_parts.hpp>

namespace network {
namespace detail {
namespace {
enum class uri_state {
  scheme,
  hier_part,
  query,
  fragment
};

enum class hier_part_state {
  first_slash,
  second_slash,
  authority,
  host,
  path
};

bool validate_scheme(string_view::const_iterator &it,
                     string_view::const_iterator last) {
  if (it == last) {
    return false;
  }

  // The first character must be a letter
  if (!std::isalpha(*it, std::locale("C"))) {
    return false;
  }
  ++it;

  while (it != last) {
    if (*it == ':') {
      break;
    }
    else if (!isalnum(it, last) && !is_in(it, last, "+-.")) {
      return false;
    }
  }

  return true;
}

bool validate_user_info(string_view::const_iterator it,
                        string_view::const_iterator last) {
  while (it != last) {
    if (!is_unreserved(it, last) &&
        !is_pct_encoded(it, last) &&
        !is_sub_delim(it, last) &&
        !is_in(it, last, ":")) {
      return false;
    }
  }

  return true;
}

// bool validate_ipv4_address(string_view::const_iterator it,
//                            string_view::const_iterator last) {
//   std::string addr(it, last);
//   string_view::value_type buffer[sizeof(struct in6_addr)];
//   int rc = ::inet_pton(AF_INET, addr.data(), &buffer);
//   return rc > 0;
// }

bool validate_ipv6_address(string_view::const_iterator it,
                           string_view::const_iterator last) {
  // the maximum ipv6 address length is 45 - choose a buffer on the
  // stack large enough
  char addr[64];
  std::memset(addr, 0, sizeof(addr));
  // copy the address with out the square braces
  std::copy(++it, --last, std::begin(addr));
  string_view::value_type buffer[sizeof(struct in6_addr)];
  int rc = ::inet_pton(AF_INET6, addr, &buffer);
  return rc > 0;
}

bool set_host(string_view::const_iterator first,
              string_view::const_iterator last,
              uri_parts &parts) {
  auto valid_host = !(*first == ':');
  if (valid_host) {
    parts.host = uri_part(first, last);
  }
  return valid_host;
}

bool set_ipv6_host(string_view::const_iterator first,
                   string_view::const_iterator last,
                   uri_parts &parts) {
  if (*first == '[') {
    auto last_but_one = last;
    --last_but_one;
    if (*last_but_one == ']') {
      if (!validate_ipv6_address(first, last)) {
        return false;
      }

      parts.host = uri_part(first, last);
    }
    else {
      return false;
    }
  }
  return true;
}

bool set_host_and_port(string_view::const_iterator first,
                       string_view::const_iterator last,
                       string_view::const_iterator last_colon,
                       uri_parts &parts) {
  if (first >= last_colon) {
    return set_host(first, last, parts);
  }
  else {
    if (*first == '[') {
      return set_ipv6_host(first, last, parts);
    }
    else {
      parts.host = uri_part(first, last_colon);

      auto port_start = last_colon;
      ++port_start;
      if ((port_start != last) && !is_valid_port(port_start, last)) {
        return false;
      }
      parts.port = uri_part(port_start, last);
    }
  }
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
    // move past the scheme delimiter
    ++it;
    state = uri_state::hier_part;
  }
  else {
    return false;
  }

  // Hierarchical part
  auto hp_state = hier_part_state::first_slash;
  // this is used by the user_info/port
  auto last_colon = first;
  while (it != last) {
    if (hp_state == hier_part_state::first_slash) {
      if (*it == '/') {
        hp_state = hier_part_state::second_slash;
        // set the first iterator in case the second slash is not forthcoming
        first = it;
        ++it;
        continue;
      }
      else {
        hp_state = hier_part_state::path;
        first = it;
      }
    }
    else if (hp_state == hier_part_state::second_slash) {
      if (*it == '/') {
        hp_state = hier_part_state::authority;
        ++it;
        first = it;
        continue;
      }
      else {
        // it's a valid URI, and this is the beginning of the path
        hp_state = hier_part_state::path;
      }
    }
    else if (hp_state == hier_part_state::authority) {
      // reset the last colon
      if (first == it) {
        last_colon = first;
      }

      if (*it == '@') {
        if (!validate_user_info(first, it)) {
          return false;
        }
        parts.user_info = uri_part(first, it);
        hp_state = hier_part_state::host;
        ++it;
        first = it;
        continue;
      }
      else if (*it == ':') {
        last_colon = it;
      }
      else if (*it == '/') {
        // we skipped right past the host and port, and are at the path.
        if (!set_host_and_port(first, it, last_colon, parts)) {
          return false;
        }
        hp_state = hier_part_state::path;
        first = it;
        continue;
      }
      else if (*it == '?') {
        // the path is empty, but valid, and the next part is the query
        if (!set_host_and_port(first, it, last_colon, parts)) {
          return false;
        }
        parts.path = uri_part(it, it);
        state = uri_state::query;
        ++it;
        first = it;
        break;
      }
      else if (*it == '#') {
        // the path is empty, but valid, and the next part is the fragment
        if (!set_host_and_port(first, it, last_colon, parts)) {
          return false;
        }
        parts.path = uri_part(it, it);
        state = uri_state::fragment;
        ++it;
        first = it;
        break;
      }
    }
    else if (hp_state == hier_part_state::host) {
      if (*first == ':') {
        return false;
      }

      if (*it == ':') {
        last_colon = it;
      }
      else if (*it == '/') {
        if (!set_host_and_port(first, it, last_colon, parts)) {
          return false;
        }

        hp_state = hier_part_state::path;
        first = it;
        continue;
      }
      else if (*it == '?') {
        // the path is empty, but valid, and the next part is the query
        if (!set_host_and_port(first, it, last_colon, parts)) {
          return false;
        }

        parts.path = uri_part(it, it);
        state = uri_state::query;
        ++it;
        first = it;
        break;
      }
      else if (*it == '#') {
        // the path is empty, but valid, and the next part is the fragment
        if (!set_host_and_port(first, it, last_colon, parts)) {
          return false;
        }

        parts.path = uri_part(it, it);
        state = uri_state::fragment;
        ++it;
        first = it;
        break;
      }
    }
    else if (hp_state == hier_part_state::path) {
      if (*it == '?') {
        parts.path = uri_part(first, it);
        // move past the query delimiter
        ++it;
        first = it;
        state = uri_state::query;
        break;
      }
      else if (*it == '#') {
        parts.path = uri_part(first, it);
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
  if (state == uri_state::hier_part) {
    if (hp_state == hier_part_state::authority) {
      if (first == last) {
        return false;
      }

      if (!set_host_and_port(first, last, last_colon, parts)) {
        return false;
      }
      parts.path = uri_part(last, last);
    }
    else if (hp_state == hier_part_state::host) {
      if (first == last) {
        return false;
      }

      if (!set_host_and_port(first, last, last_colon, parts)) {
        return false;
      }
      parts.path = uri_part(last, last);
    }
    else if (hp_state == hier_part_state::path) {
      parts.path = uri_part(first, last);
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
