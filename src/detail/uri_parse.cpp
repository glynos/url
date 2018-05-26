// Copyright 2016-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <arpa/inet.h>
#include "network/uri/detail/uri_parse.hpp"
#include "network/uri/detail/encode.hpp"
#include "grammar.hpp"
#include "detail/url_schemes.hpp"

#include <iostream>

namespace network {
namespace detail {
namespace {
void remove_leading_whitespace(std::string &input, url_result &result) {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(first, last, [](char ch) -> bool {
    return
        !(std::isspace(ch, std::locale()) ||
        ch == '\0' || ch == '\x1b' || ch == '\x04' || ch == '\x12' || ch == '\x1f');
  });
  if (it != first) {
    result.validation_error = true;
    input = std::string(it, last);
  }
}

void remove_trailing_whitespace(std::string &input, url_result &result) {
  using reverse_iterator = std::reverse_iterator<std::string::const_iterator>;

  auto first = reverse_iterator(end(input)),
      last = reverse_iterator(begin(input));
  auto it = std::find_if(first, last, [](char ch) -> bool {
    return
        !(std::isspace(ch, std::locale()) ||
        ch == '\0' || ch == '\x1b' || ch == '\x04' || ch == '\x12' || ch == '\x1f');
  });
  if (it != first) {
    result.validation_error = true;
    input = std::string(it, last);
    std::reverse(begin(input), end(input));
  }
}

void remove_tabs_and_newlines(std::string &input, url_result &result) {
  auto it = std::remove_if(begin(input), end(input),
                           [] (char c) -> bool { return (c == '\t') || (c == '\n'); });
  result.validation_error = (it != end(input));
  input.erase(it, end(input));
}

inline bool is_special_scheme(const std::string &scheme) {
  return is_special(string_view(scheme.data(), scheme.length() - 1));
}

inline bool is_forbidden_host_point(string_view::value_type c) {
  static const char forbidden[] = "\0\t\n\r #%/:?@[\\]";
  const char *first = forbidden, *last = forbidden + sizeof(forbidden);
  return last != std::find(first, last, c);
}

bool remaining_starts_with(string_view::const_iterator first,
                           string_view::const_iterator last,
                           const char *chars) {
  auto chars_first = chars, chars_last = chars + std::strlen(chars);
  auto chars_it = chars_first;
  auto it = first;
  ++it;
  while (chars_it != chars_last) {
    if (*it != *chars_it) {
      return false;
    }

    ++it;
    ++chars_it;

    if (it == last) {
      return false;
    }
  }
  return true;
}

bool validate_domain(const std::string &buffer) {
  auto view = string_view(buffer);
  auto first = begin(view), last = end(view);

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

inline bool validate_ipv4_address(const std::string &buffer) {
  auto view = string_view(buffer);
  auto first = begin(view), last = end(view);
  return validate_ip_address(first, last, AF_INET);
}

inline bool validate_ipv6_address(const std::string &buffer) {
  auto view = string_view(buffer);
  auto first = begin(view), last = end(view);
  return validate_ip_address(++first, --last, AF_INET6);
}

bool set_ipv4_host(const std::string &buffer,
                   std::string &hostname) {
  auto view = string_view(buffer);
  auto first = begin(view), last = end(view);
  auto valid_host = (last == std::find_if(first, last, is_forbidden_host_point));
  if (valid_host) {
    valid_host =
      validate_domain(buffer) ||
      validate_ipv4_address(buffer)
      ;

    if (valid_host) {
      hostname = buffer;
    }
  }
  return valid_host;
}

bool set_ipv6_host(const std::string &buffer,
                   std::string &hostname) {
  auto view = string_view(buffer);
  auto first = begin(view), last = end(view);

  if (*first != '[') {
    return false;
  }

  auto last_but_one = last;
  --last_but_one;
  if (*last_but_one != ']') {
    return false;
  }

  if (!validate_ipv6_address(buffer)) {
    return false;
  }

  hostname.assign(first, last);
  return true;
}

bool set_host(
    const std::string &scheme,
    const std::string &buffer,
    std::string &hostname) {
  if (buffer.front() == '[') {
    if (buffer.back() != ']') {
      // result.validation_error = true;
      return false;
    }
    return set_ipv6_host(buffer, hostname);
  }


  auto domain = std::string();
  encode_host(begin(buffer), end(buffer), std::back_inserter(domain));
  auto it = std::find_if(begin(domain), end(domain), is_forbidden_host_point);
  if (it != end(domain)) {
    // result.validation_error = true;
    return false;
  }

  return set_ipv4_host(domain, hostname);
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

bool is_windows_drive_letter(
    string_view::const_iterator it,
    string_view::const_iterator last) {
  if (std::distance(it, last) < 2) {
    return false;
  }

  if (!std::isalpha(*it)) {
    return false;
  }

  ++it;
  if ((*it != ':') || (*it != '|')) {
    return false;
  }

  return true;
}

inline bool is_windows_drive_letter(const std::string &el) {
  auto view = string_view(el);
  return is_windows_drive_letter(begin(view), end(view));
}

void shorten_path(std::vector<std::string> &path, url_result &result) {
  if (path.empty()) {
    return;
  }

  if ((result.scheme == "file:") && (path.size() == 1) && is_windows_drive_letter(path.front())) {
    return;
  }

  path.pop_back();
}

void set_query(const std::string &buffer, std::string &query) {
  if (buffer.empty()) {
    return;
  }

  auto first = begin(buffer), last = end(buffer);
  query += '?';
  encode_query(first, last, back_inserter(query));
}

void set_fragment(const std::string &buffer, std::string &fragment) {
  if (buffer.empty()) {
    return;
  }

  auto first = begin(buffer), last = end(buffer);
  fragment += '#';
  encode_fragment(first, last, std::back_inserter(fragment));
}
} // namespace

url_result parse(
    std::string input,
    const optional<url_result> &base,
    const optional<url_result> &url,
    url_state state_override) {
  auto result = url? url.value() : url_result{};


//   std::cout << "URL: " << input << "\r\n" << std::endl;
  if (input == "lolscheme:x x#x x") {
    std::cout << "OK!" << "\r\n" << std::endl;
  }


  if (input.empty()) {
    return result;
  }

  remove_leading_whitespace(input, result);
  remove_trailing_whitespace(input, result);
  remove_tabs_and_newlines(input, result);

  auto state = (state_override == url_state::null)? url_state::scheme_start : state_override;

  auto buffer = std::string();

  auto at_flag = false;
  auto square_brace_flag = false;
  auto password_token_seen_flag = false;

  auto view = string_view(input);
  auto first = begin(view), last = end(view);
  auto it = first;

  while (it != last) {
    if (state == url_state::scheme_start) {
      if (std::isalpha(*it) != 0) {
        auto lower = std::tolower(*it);
        buffer.push_back(static_cast<char>(lower));
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
        auto lower = std::tolower(*it);
        buffer.push_back(static_cast<char>(lower));
      }
      else if (*it == ':') {
        buffer.push_back(*it);
        result.scheme = buffer;
        buffer.clear();

        if (result.scheme.compare("file:") == 0) {
          auto next = it;
          ++next;
          if (!is_double_slash(next, last)) {
            result.validation_error = true;
          }
          state = url_state::file;
        }
//        else if (is_special_scheme(result.scheme)) {
//          // if base is non-null, base's scheme is equal to this, else
//        }
        else if (is_special_scheme(result.scheme)) {
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
            result.path.push_back(std::string());
          }
        }
      }
      else if (state_override == url_state::null) {
        buffer.clear();
        state = url_state::no_scheme;
        it = first;
        continue;
      }
    }
    else if (state == url_state::no_scheme) {
      state = url_state::file;
      --it;
    }
    else if (state == url_state::special_relative_or_authority) {
      if ((*it == '/') && remaining_starts_with(it, last, "/")) {
        ++it;
        state = url_state::special_authority_ignore_slashes;
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
      }
      else {
        state = url_state::path;
        --it;
      }
    }
    else if (state == url_state::relative) {

    }
    else if (state == url_state::relative_slash) {

    }
    else if (state == url_state::special_authority_slashes) {
      if ((*it == '/') && remaining_starts_with(it, last, "/")) {
        ++it;
        state = url_state::special_authority_ignore_slashes;
      }
      else {
        result.validation_error = true;
        --it;
        state = url_state::special_authority_ignore_slashes;
      }
    }
    else if (state == url_state::special_authority_ignore_slashes) {
      if ((*it != '/') && (*it != '\\')) {
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

          // TODO: percent encode

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
        continue;
      }
      else if ((*it == '/') || (*it == '?') || (*it == '#') ||
               (is_special_scheme(result.scheme) && (*it == '\\'))) {
        if (at_flag && buffer.empty()) {
          result.validation_error = true;
          return result;
        }
        it = it - buffer.size();
        state = url_state::host;
        buffer.clear();
        continue;
      }
      else {
        buffer.push_back(*it);
      }
    }
    else if (state == url_state::host) {
      if ((state_override != url_state::null) && (result.scheme.compare("file:") == 0)) {
        --it;
        state = url_state::file_host;
      }
      else if ((*it == ':') && !square_brace_flag) {
        if (buffer.empty()) {
          result.validation_error = true;
          return result;
        }

        if (!set_host(result.scheme, buffer, result.hostname)) {
          return result;
        }
        buffer.clear();

        state = url_state::port;

        if (state_override == url_state::hostname) {
          result.success = true;
          return result;
        }
      }
      else if ((*it == '/') || (*it == '?') || (*it == '#') ||
               (is_special_scheme(result.scheme) && (*it == '\\'))) {
        --it;

        if (is_special_scheme(result.scheme) && buffer.empty()) {
          result.validation_error = true;
          return result;
        }

        if (!set_host(result.scheme, buffer, result.hostname)) {
          return result;
        }
        buffer.clear();

        state = url_state::path;
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
          if (!is_valid_port(buffer)) {
            result.validation_error = true;
            return result;
          }

          auto view = string_view(result.scheme.data(), result.scheme.length() - 1);
          result.port = is_default_port(view, std::atoi(buffer.c_str()))? "" : buffer;
          buffer.clear();
        }

        if (state_override != url_state::null) {
          result.success = true;
          return result;
        }

        --it;
        state = url_state::path_start;
      }
      else {
        result.validation_error = true;
        return result;
      }
    }
    else if (state == url_state::file) {
      result.scheme = "file:";

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
      if ((*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#')) {
        --it;

        if ((state_override != url_state::null) && (is_windows_drive_letter(it, last))) {
          result.validation_error = true;
          state = url_state::path;
        }
        else if (buffer.empty()) {
          result.hostname.clear();

          if (state_override != url_state::null) {
            result.success = true;
            return result;
          }

          state = url_state::path_start;
        }
        else {
          // TODO: parse host
          if (!set_host(result.scheme, buffer, result.hostname)) {
            return result;
          }

          if (result.hostname == "localhost") {
            result.hostname.clear();
          }

          if (state_override != url_state::null) {
            result.success = true;
            return result;
          }

          buffer.clear();

          state = url_state::path_start;
        }
      }
      else {
        buffer.push_back(*it);
      }
    }
    else if (state == url_state::path_start) {
      if (is_special_scheme(result.scheme)) {
        if (*it == '\\') {
          result.validation_error = true;
        }
        state = url_state::path;
      }
      else if (state_override == url_state::null) {
        if (*it == '?') {
          result.query.clear();
          state = url_state::query;
        }
        else if (*it == '#') {
          result.fragment.clear();
          state = url_state::fragment;
        }
      }
      else {
        state = url_state::path;
        if (*it != '/') {
          --it;
        }
      }
    }
    else if (state == url_state::path) {
      if ((*it == '/') ||
          (is_special_scheme(result.scheme) && (*it == '\\')) ||
          ((state_override == url_state::null) && ((*it == '?') || (*it == '#')))) {
        if (is_special_scheme(result.scheme) && (*it == '\\')) {
          result.validation_error = true;
        }
        else if (buffer == "..") {
          shorten_path(result.path, result);
          if ((*it != '/') && (is_special_scheme(result.scheme) && (*it == '\\'))) {
            result.path.push_back(std::string());
          }
        }
        else if ((buffer == ".") && ((*it != '/') && (is_special_scheme(result.scheme) && (*it == '\\')))) {
          result.path.push_back(std::string());
        }
        else if (buffer != ".") {
          if ((result.scheme == "file:") && result.path.empty() && is_windows_drive_letter(buffer)) {
            if (result.hostname.empty()) {
              result.validation_error = true;
              result.hostname.clear();
            }
            buffer[1] = ':';
          }
          else {
            result.path.push_back(buffer);
          }
        }

        buffer.clear();

        if (*it == '?') {
          result.query.clear();
          state = url_state::query;
        }

        if (*it == '#') {
          result.fragment.clear();
          state = url_state::fragment;
        }
      }
      else {
        buffer += *it;
      }
    }
    else if (state == url_state::cannot_be_a_base_url_path) {
      if (*it == '?') {
        result.query.clear();
        state = url_state::query;
      }
      else if (*it == '#') {
        result.fragment.clear();
        state = url_state::fragment;
      }
      else if (*it != '%') {
        result.validation_error = true;
      }
//      else {
//        result.path[0].push_back(*it);
//      }
    }
    else if (state == url_state::query) {
      if (*it == '#') {
        set_query(buffer, result.query);

        buffer.clear();
        state = url_state::fragment;
      }
      else {
        buffer += *it;
      }
    }
    else if (state == url_state::fragment) {
      if (*it == '\0') {
        result.validation_error = true;
      }
      else {
        buffer += *it;
      }
    }

    ++it;
  }

  // we're done!
  if (state == url_state::authority) {
    state = url_state::host;
  }

  if (state == url_state::host) {
    if (is_special_scheme(result.scheme) && buffer.empty()) {
      result.validation_error = true;
      return result;
    }

    if (!set_host(result.scheme, buffer, result.hostname)) {
      return result;
    }
    buffer.clear();

    state = url_state::path;
  }

  if (state == url_state::port) {

    state = url_state::path;
  }

  if (state == url_state::path) {
    if (buffer == "..") {
      shorten_path(result.path, result);
      if ((*it != '/') && (is_special_scheme(result.scheme) && (*it == '\\'))) {
        result.path.push_back(std::string());
      }
    }
    else if ((buffer == ".") && ((*it != '/') && (is_special_scheme(result.scheme) && (*it == '\\')))) {
      result.path.push_back(std::string());
    }
    else if (buffer != ".") {
      if ((result.scheme == "file:") && result.path.empty() && is_windows_drive_letter(buffer)) {
        if (result.hostname.empty()) {
          result.validation_error = true;
          result.hostname.clear();
        }
        buffer[1] = ':';
      } else {
        result.path.push_back(buffer);
      }
    }
  }

  if (state == url_state::query) {
    set_query(buffer, result.query);
  }

  if (state == url_state::fragment) {
    set_fragment(buffer, result.fragment);
  }

  result.success = true;
  return result;
}
}  // namespace detail
}  // namespace network
