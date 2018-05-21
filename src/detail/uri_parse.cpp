// Copyright 2016-2018 Glyn Matthews.
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

#include <iostream>

namespace network {
namespace detail {
namespace {
bool validate_scheme(string_view::const_iterator &it,
                     string_view::const_iterator last,
                     url_state state_override) {
  if (it == last) {
    return false;
  }

  while (it != last) {
    if (*it == ':') {
      ++it;
      break;
    }
    else if (!isalnum(*it) && !is_in(*it, "+-.")) {
      return false;
    }

//    *it = static_cast<char>(std::tolower(static_cast<int>(*it)));
    ++it;
  }

  return true;
}
  
bool is_special_scheme(string_view scheme) {
  return is_special(scheme.substr(0, scheme.length() - 1));
}

bool validate_user_info(string_view::const_iterator first,
                        string_view::const_iterator last) {
  if (first == last) {
    return true;
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
  static const char forbidden[] = "\0\t\n\r #%/:?@[\\]";
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

bool set_ipv4_host(string_view::const_iterator first,
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

bool set_host(string_view scheme,
              string_view::const_iterator first,
              string_view::const_iterator last,
              uri_parts &parts) {
  if ((scheme.compare("file:") != 0) && (first == last)) {
    return false;
  }

  if (*first == '[') {
    return set_ipv6_host(first, last, parts);
  }
  else {
    if (!set_ipv4_host(first, last, parts)) {
      return false;
    }
  }
  return true;
}

//bool set_port(string_view scheme,
//              string_view::const_iterator port_first,
//              string_view::const_iterator port_last,
//              uri_parts &parts) {
//  if (port_first != port_last) {
//    if (scheme.compare("file:") == 0) {
//      return false;
//    }
//
//    if (!is_valid_port(port_first, port_last)) {
//      return false;
//    }
//  }
//
//  parts.port = uri_part(port_first, port_last);
//  return true;
//}

//bool set_host_and_port(string_view scheme,
//                       string_view::const_iterator first,
//                       string_view::const_iterator last,
//                       string_view::const_iterator last_colon,
//                       uri_parts &parts) {
//  if ((scheme.compare("file:") != 0) && (first == last)) {
//    return false;
//  }
//
//  if (first >= last_colon) {
//    return set_ipv4_host(first, last, parts);
//  }
//  else {
//    if (*first == '[') {
//      return set_ipv6_host(first, last, parts);
//    }
//    else {
//      if (!set_ipv4_host(first, last_colon, parts)) {
//        return false;
//      }
//
//      auto port_first = last_colon;
//      ++port_first;
//      if (!set_port(scheme, port_first, last, parts)) {
//        return false;
//      }
//    }
//  }
//  return true;
//}

bool set_path(string_view::const_iterator first,
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
  if (*it != '#') {
    return false;
  }
  ++it;

  while (it != last) {
    if (!is_pchar(it, last) && !is_in(it, last, "?/\\ \t")) {
      return false;
    }
  }
  return true;
}
} // namespace

bool parse(string_view::const_iterator &it, string_view::const_iterator last,
           uri_parts &parts, url_state state_override) {
  if (it == last) {
    return false;
  }

  auto first = it;

  auto state = url_state::scheme_start;

  std::cout << "URL: " << std::string(first, last) << "\r\n" << std::endl;
  if (std::string(first, last) == "https://[0::0::0]") {
    std::cout << "OK!" << "\r\n" << std::endl;
  }

  // The first character must be a letter
  if (std::isalpha(*it) != 0) {
    state = url_state::scheme;
    ++it;
  }
  else if (state_override == url_state::null) {
    state = url_state::no_scheme;
  }
  else {
    return false;
  }

  if (state == url_state::scheme) {
    if (validate_scheme(it, last, state_override)) {
      parts.scheme = uri_part(first, it);
      if (state_override != url_state::null) {

      }

      if (string_view(*parts.scheme).substr(0, 4).compare("file") == 0) {
        auto slash_it = it;
        for (auto i = 0; i < 2; ++i) {
          if ((*slash_it == '/') || (*slash_it == '\\')) {
            if (*slash_it == '\\') {
              // validation error
              break;
            }
            ++slash_it;
          } else {
            // validation error
            break;
          }
        }

        state = url_state::file;
      }
//      else if (is_special_scheme(static_cast<string_view>(*parts.scheme)) &&
//               (state_override == url_state::null)) {
//        state = url_state::special_relative_or_authority;
//      }
      else if (is_special_scheme(static_cast<string_view>(*parts.scheme))) {
        // if base is non-null, base's scheme is equal to this, else
        state = url_state::special_authority_slashes;
      }
      else if (string_view(std::addressof(*it), std::distance(it, last)).compare("/") == 0) {
        state = url_state::path_or_authority;
      }
      else {
        state = url_state::cannot_be_a_base_url_path;
      }
    }
    else if (state_override == url_state::null) {
      it = first;
      ++it;
      state = url_state::no_scheme;
    }
    else {
      return false;
    }
  }

  if (state == url_state::no_scheme) {

  }

  // Hierarchical part
  // this is used by the user_info/port
  while (it != last) {
    if (state == url_state::special_relative_or_authority) {
      if (*it == '/') {
        auto slash_it = it;
        ++slash_it;
        if (*slash_it == '/') {
          ++it;
          state = url_state::special_authority_ignore_slashes;
        }
        else {
          // validation error
          --it;
          state = url_state::relative;
        }
      }
      else {
        // validation error
        --it;
        state = url_state::relative;
      }
    }
    else if (state == url_state::special_authority_slashes) {
      if (*it == '/') {
        auto slash_it = it;
        ++slash_it;
        if (*slash_it == '/') {
          ++it;
          state = url_state::special_authority_ignore_slashes;
        }
        else {
          // validation error
          --it;
          state = url_state::special_authority_ignore_slashes;
        }
      }
      else {
        // validation error
        --it;
        state = url_state::special_authority_ignore_slashes;
      }
    }
    else if (state == url_state::special_authority_ignore_slashes) {
      if ((*it != '/') && (*it != '\\')) {
        first = it;
        --it;
        state = url_state::authority;
      }
      else {
        // validation error
      }
    }
    else if (state == url_state::authority) {
      if (*it == '@') {
        if (!validate_user_info(first, it)) {
          return false;
        }
        parts.user_info = uri_part(first, it);
        state = url_state::host;
        ++it;
        first = it;
        continue;
      }
      else if ((*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#')) {
        state = url_state::host;
        it = first;
        continue;
      }
    }
    else if (state == url_state::host) {
      if (*first == ':') {
        return false;
      }

      if (*it == ':') {
        state = url_state::port;
        first = it;
        ++first;
      }
      else if ((*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#')) {
        if (!set_host(static_cast<string_view>(*parts.scheme), first, it, parts)) {
          return false;
        }

        state = url_state::path;
        first = it;
        continue;
      }
    }
    else if (state == url_state::port) {
      if (std::isdigit(static_cast<int>(*it)) == 0) {
        if ((*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#')) {
          if (first == it) {
            // set default port for scheme
            parts.port = uri_part(first, it);
          }
          else if (is_valid_port(first, it)) {
            parts.port = uri_part(first, it);
          }
          else {
            return false;
          }
          state = url_state::path;
          first = it;
          continue;
        }
        else {
          return false;
        }
      }
    }
    else if (state == url_state::file) {
      if ((*it == '/') || (*it == '\\')) {
        if (*it == '\\') {
          return false;
        }
        state = url_state::file_slash;
      }
      else {
        --it;
        state = url_state::path;
      }
    }
    else if (state == url_state::file_slash) {
      if ((*it == '/') || (*it == '\\')) {
        if (*it == '\\') {
          return false;
        }
        state = url_state::file_host;
      }
      else {
        --it;
        state = url_state::path;
      }
    }
    else if (state == url_state::file_host) {
      auto host_first = it;

      while (!((*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#') || (it == last))) {
        ++it;
      }

      if (!set_host(static_cast<string_view>(*parts.scheme), host_first, it, parts)) {
        return false;
      }
      --it;
      state = url_state::path_start;
    }
    else if (state == url_state::path_start) {
      if (is_special_scheme(static_cast<string_view>(*parts.scheme))) {
        if (*it == '\\') {
          return false;
        }
        else {
          state = url_state::path;
        }
      }
      else if (state_override == url_state::null) {
        if (*it == '?') {
          state = url_state::query;
        }
        else if (*it == '#') {
          state = url_state::fragment;
        }
      }
    }
    else if (state == url_state::path) {
      if (*it == '?') {
        if (!set_path(first, it, parts)) {
          return false;
        }

        first = it;
        state = url_state::query;
        break;
      }
      else if (*it == '#') {
        if (!set_path(first, it, parts)) {
          return false;
        }


        first = it;
        state = url_state::fragment;
        break;
      }

      if (!is_pchar(it, last) && !is_in(it, last, "/\\ \t")) {
        return false;
      }
      else {
        continue;
      }
    }

    ++it;
  }

  if (state == url_state::query) {
    while (it != last) {
      if (!is_pchar(it, last) && !is_in(it, last, "?/\\")) {
        // If this is a fragment, keep going
        if (*it == '#') {
          parts.query = uri_part(first, it);
          first = it;
          state = url_state::fragment;
          break;
        }
        else {
          return false;
        }
      }
    }
  }

  if (state == url_state::fragment) {
    if (!validate_fragment(it, last)) {
      return false;
    }
  }

  // we're done!
//  if ((state == url_state::first_slash) ||
//      (state == url_state::second_slash)) {
//    return false;
//  }
  if (state == url_state::authority) {
    if (first == last) {
      return false;
    }

    if (!set_host(static_cast<string_view>(*parts.scheme), first,
                           last, parts)) {
      return false;
    }

    if (!set_path(last, last, parts)) {
      return false;
    }
  }
  else if (state == url_state::host) {
    if (first == last) {
      return false;
    }

    if (!set_host(static_cast<string_view>(*parts.scheme), first,
                           last, parts)) {
      return false;
    }

    if (!set_path(last, last, parts)) {
      return false;
    }
  }
  else if (state == url_state::path) {
    if (!set_path(first, last, parts)) {
      return false;
    }
  }
  else if (state == url_state::query) {
    parts.query = uri_part(first, last);
  }
  else if (state == url_state::fragment) {
    parts.fragment = uri_part(first, last);
  }

  return true;
}
}  // namespace detail
}  // namespace network
