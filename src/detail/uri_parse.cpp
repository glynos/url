// Copyright 2016-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <arpa/inet.h>
#include "network/uri/detail/uri_parse.hpp"
#include "grammar.hpp"
#include "detail/url_schemes.hpp"

#include <iostream>

namespace network {
namespace detail {
namespace {
inline bool is_special_scheme(const std::string &scheme) {
  return is_special(string_view(scheme.data(), scheme.length() - 1));
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
                   std::string &hostname) {
  auto valid_host = (last == std::find_if(first, last, is_forbidden_host_point));
  if (valid_host) {
    valid_host =
      validate_domain(first, last) ||
      validate_ipv4_address(first, last)
      ;

    if (valid_host) {
      hostname.assign(first, last);
    }
  }
  return valid_host;
}

bool set_ipv6_host(string_view::const_iterator first,
                   string_view::const_iterator last,
                   std::string &hostname) {
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

  hostname.assign(first, last);
  return true;
}

bool set_host(
    const std::string &scheme,
    string_view::const_iterator first,
    string_view::const_iterator last,
    std::string &hostname) {
  if ((scheme.compare("file:") != 0) && (first == last)) {
    return false;
  }

  if (*first == '[') {
    return set_ipv6_host(first, last, hostname);
  }
  else {
    if (!set_ipv4_host(first, last, hostname)) {
      return false;
    }
  }
  return true;
}

bool set_path(string_view::const_iterator first,
              string_view::const_iterator last,
              std::string &path) {
  if (first != last) {
    if ((*first == '@') ||
        (*first == ':')) {
      return false;
    }
  }

  path.assign(first, last);
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

bool is_double_slash(
    string_view::const_iterator it,
    string_view::const_iterator last) {
  if (std::distance(it, last) < 2) {
    return false;
  }

  auto slash_it = it;
  for (auto i = 0; i < 2; ++i) {
    if (*slash_it == '/') {
      ++slash_it;
    }
    else {
      return false;
    }
  }
  return true;
}
} // namespace

url_result parse(string_view::const_iterator &it, string_view::const_iterator last,
                 url_state state_override) {
  auto result = url_result{};

  if (it == last) {
    return result;
  }

  auto first = it;

  auto buffer = std::string();

  auto at_flag = false;
  auto square_brace_flag = false;
  auto password_token_seen_flag = false;

  auto state = url_state::scheme_start;

  // std::cout << "URL: " << std::string(first, last) << "\r\n" << std::endl;
  if (std::string(it, last) == "httpa://foo:80/") {
    std::cout << "OK!" << "\r\n" << std::endl;
  }

  while (it != last) {
    if (state == url_state::scheme_start) {
      if (std::isalpha(*it) != 0) {
        buffer.push_back(static_cast<char>(std::tolower(static_cast<int>(*it))));
        state = url_state::scheme;
      }
      else if (state_override == url_state::null) {
        state = url_state::no_scheme;
      }
      else {
        result.validation_error = true;
        return result;
      }
    }
    else if (state == url_state::scheme) {
      if ((std::isalnum(static_cast<int>(*it)) != 0) || is_in(*it, "+-.")) {
        buffer.push_back(static_cast<char>(std::tolower(static_cast<int>(*it))));
      }
      else if (*it == ':') {
        buffer.push_back(*it);
        result.scheme = buffer;
        buffer.clear();

        if (result.scheme.compare("file:") == 0) {
          if (!is_double_slash(it, last)) {
            result.validation_error = true;
          }
          state = url_state::file;
        }
        else if (is_special_scheme(result.scheme)) {
          // if base is non-null, base's scheme is equal to this, else
          state = url_state::special_authority_slashes;
        }
        else {
          auto next = it;
          ++next;
          if ((next != last) && (*next == '/')) {
            state = url_state::path_or_authority;
            ++it;
          }
          else {
            state = url_state::cannot_be_a_base_url_path;
          }
        }
      }
    }
    else if (state == url_state::no_scheme) {

    }
    else if (state == url_state::special_relative_or_authority) {
      if (*it == '/') {
        auto slash_it = it;
        ++slash_it;
        if (*slash_it == '/') {
          ++it;
          state = url_state::special_authority_ignore_slashes;
        }
        else {
          result.validation_error = true;
          --it;
          state = url_state::relative;
        }
      }
      else {
        result.validation_error = true;
        --it;
        state = url_state::relative;
      }
    }
    else if (state == url_state::path_or_authority) {
      if (*it == '/') {
        state = url_state::authority;
        first = it;
      }
      else {
        state = url_state::path;
        --it;
      }
    }
    else if (state == url_state::relative) {

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
          result.validation_error = true;
          --it;
          state = url_state::special_authority_ignore_slashes;
        }
      }
      else {
        result.validation_error = true;
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
        result.validation_error = true;
      }
    }
    else if (state == url_state::authority) {
      if (*it == '@') {
        result.validation_error = true;
        if (at_flag) {
          buffer = "%40" + buffer;
        }
        at_flag = true;

        for (auto c : buffer) {
          if (c == ':' && !password_token_seen_flag) {
            password_token_seen_flag = true;
            continue;
          }

          if (password_token_seen_flag) {
            result.password += c;
          }
          else {
            result.username += c;
          }
        }
        buffer.clear();

        state = url_state::host;
        ++it;
        first = it;
        continue;

      }
      else if ((*it == '/') || (*it == '?') || (*it == '#') ||
               (is_special_scheme(result.scheme) && (*it == '\\'))) {
        if (at_flag && buffer.empty()) {
          result.validation_error = true;
        }
        it = first;
        state = url_state::host;
        continue;
      }
      else {
        buffer.push_back(*it);
      }
    }
    else if (state == url_state::host) {
      if (state_override != url_state::null) {
        if (result.scheme.compare("file:") == 0) {
          --it;
          state = url_state::file_host;
        }
      }

      if ((*it == ':') && !square_brace_flag) {
        if (buffer.empty()) {
          result.validation_error = true;
          return result;
        }

        if (!set_host(result.scheme, first, it, result.hostname)) {
          return result;
        }
        buffer.clear();

        state = url_state::port;
        first = it;
        ++first;

        if (state_override == url_state::hostname) {
          result.success = true;
          return result;
        }
      }
      else if ((*it == '/') || (*it == '?') || (*it == '#') ||
               (is_special_scheme(result.scheme) && (*it == '\\'))) {
        if (!set_host(result.scheme, first, it, result.hostname)) {
          return result;
        }

        state = url_state::path;
        first = it;
        continue;
      }
      else {
        if (*it == '[') {
          square_brace_flag = true;
        }
        else if (*it == ']') {
          square_brace_flag = false;
        }
        buffer += *it;
      }
    }
    else if (state == url_state::port) {
      if (std::isdigit(static_cast<int>(*it)) != 0) {
        buffer += *it;
      }
      else if ((*it == '/') || (*it == '?') || (*it == '#') ||
               (is_special_scheme(result.scheme) && (*it == '\\')) ||
               (state_override != url_state::null)) {
        if (!buffer.empty()) {
          if (is_valid_port(first, it)) {
            if (!is_default_port(string_view(result.scheme.data(), result.scheme.length() - 1),
                                 std::atoi(buffer.c_str()))) {
              result.port = buffer;
            }
          }
        }
        buffer.clear();

        if (state_override != url_state::null) {
          result.success = true;
          return result;
        }

        first = it;
        --it;
        state = url_state::path_start;
      }
      else {
        result.validation_error = true;
        return result;
      }
    }
    else if (state == url_state::file) {
      if ((*it == '/') || (*it == '\\')) {
        if (*it == '\\') {
          result.validation_error = true;
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
          result.validation_error = true;
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

      if (!set_host(result.scheme, host_first, it, result.hostname)) {
        return result;
      }
      --it;
      state = url_state::path_start;
    }
    else if (state == url_state::path_start) {
      if (is_special_scheme(result.scheme)) {
        if (*it == '\\') {
          return result;
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
        if (!set_path(first, it, result.path)) {
          return result;
        }

        first = it;
        state = url_state::query;
        break;
      }
      else if (*it == '#') {
        if (!set_path(first, it, result.path)) {
          return result;
        }

        first = it;
        state = url_state::fragment;
        break;
      }

      if (!is_pchar(it, last) && !is_in(it, last, "/\\ \t")) {
        return result;
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
          result.query = buffer;
//          result.parts.query = uri_part(first, it);
          // first = it;
          buffer.clear();
          state = url_state::fragment;
          break;
        }
        else {
          return result;
        }
      }
      else {
        buffer += *it;
      }
    }
  }

  if (state == url_state::fragment) {
    if (!validate_fragment(it, last)) {
      return result;
    }

    buffer.assign(it, last);
  }

  // we're done!
  if (state == url_state::authority) {
    if (first == last) {
      return result;
    }

    if (!set_host(result.scheme, first, last, result.hostname)) {
      return result;
    }

    if (!set_path(last, last, result.path)) {
      return result;
    }
  }
  else if (state == url_state::host) {
    if (first == last) {
      return result;
    }

    if (!set_host(result.scheme, first, last, result.hostname)) {
      return result;
    }

    if (!set_path(last, last, result.path)) {
      return result;
    }
  }
  else if (state == url_state::path) {
    if (!set_path(first, last, result.path)) {
      return result;
    }
  }
  else if (state == url_state::query) {
    result.query = buffer;
  }
  else if (state == url_state::fragment) {
    result.query = buffer;
  }

  result.success = true;
  return result;
}
}  // namespace detail
}  // namespace network
