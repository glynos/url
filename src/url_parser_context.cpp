// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "url_parser_context.hpp"
#include <iterator>
#include <limits>
#include <cmath>
#include <sstream>
#include <deque>
#include <map>
#include <array>
#include "skyr/url/details/encode.hpp"
#include "skyr/url/details/decode.hpp"
#include "url_schemes.hpp"
#include "skyr/url_state.hpp"
#include "skyr/ipv6_address.hpp"

namespace skyr {
namespace {
inline bool is_in(string_view::value_type c,
                  string_view view) {
  auto first = begin(view), last = end(view);
  return last != std::find(first, last, c);
}

inline bool is_whitespace(char ch) {
  const char whitespace[] = "\0\x1b\x04\x12\x1f";

  return
      !(std::isspace(ch, std::locale("C")) ||
          is_in(ch, string_view(whitespace, sizeof(whitespace))));
}

bool remove_leading_whitespace(std::string &input) {
  auto view = string_view(input);
  auto first = begin(view), last = end(view);
  auto it = std::find_if(first, last, is_whitespace);
  if (it != first) {
    input = std::string(it, last);
  }

  return it == first;
}

bool remove_trailing_whitespace(std::string &input) {
  using reverse_iterator = std::reverse_iterator<std::string::const_iterator>;

  auto first = reverse_iterator(end(input)),
      last = reverse_iterator(begin(input));
  auto it = std::find_if(first, last, is_whitespace);
  if (it != first) {
    input = std::string(it, last);
    std::reverse(begin(input), end(input));
  }

  return it == first;
}

bool remove_tabs_and_newlines(std::string &input) {
  auto first = begin(input), last = end(input);
  auto it = std::remove_if(
      first, last, [] (char c) -> bool { return is_in(c, "\t\r\n"); });
  input.erase(it, end(input));
  return it == last;
}

inline bool is_forbidden_host_point(string_view::value_type c) {
  static const char forbidden[] = "\0\t\n\r #%/:?@[\\]";
  const char *first = forbidden, *last = forbidden + sizeof(forbidden);
  return last != std::find(first, last, c);
}

bool remaining_starts_with(
    string_view::const_iterator first,
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
      return (chars_it == chars_last);
    }
  }

  return true;
}

optional<std::string> parse_opaque_host(string_view input) {
  auto it = std::find_if(
        begin(input), end(input),
        [] (char c) -> bool {
          static const char forbidden[] = "\0\t\n\r #/:?@[\\]";
          const char *first = forbidden, *last = forbidden + sizeof(forbidden);
          return last != std::__1::find(first, last, c);
        });
  if (it != end(input)) {
      // result.validation_error = true;
      return nullopt;
    }

  auto output = std::string();
  for (auto c : input) {
    details::pct_encode_char(c, back_inserter(output));
    }

  return output;
}

template <class InputIter, class OutputIter>
OutputIter domain_to_ascii(InputIter first, InputIter last, OutputIter it_out) {
  auto it = first;
  while (it != last) {
    it_out++ = std::tolower(*it, std::locale("C"));
//    it_out++ = *it;
    ++it;
  }
  return it_out;
};

optional<std::string> parse_host(string_view input, bool is_not_special = false) {
  if (input.front() == '[') {
    if (input.back() != ']') {
      // result.validation_error = true;
      return nullopt;
    }

    auto view = string_view(input);
    view.remove_prefix(1);
    view.remove_suffix(1);
    auto ipv6_address = parse_ipv6_address(view);
    if (ipv6_address) {
      return ipv6_address.value().to_string();
    }
    else {
      return nullopt;
    }
  }

  if (is_not_special) {
    return parse_opaque_host(input);
  }

  auto domain = std::string{};
  try {
    details::pct_decode(begin(input), end(input), std::back_inserter(domain));
  }
  catch (std::exception &) {
    // result.validation_error = true;
    return nullopt;
  }

  auto ascii_domain = std::string{};
  domain_to_ascii(begin(domain), end(domain), std::back_inserter(ascii_domain));

  auto it = std::find_if(begin(ascii_domain), end(ascii_domain), is_forbidden_host_point);
  if (it != end(ascii_domain)) {
    // result.validation_error = true;
    return nullopt;
  }

  auto ipv4_host = parse_ipv4_address(string_view(ascii_domain));

  auto ok = false;
  auto host = optional<ipv4_address>();

  std::tie(ok, host) = ipv4_host;

  if (!ok) {
    return nullopt;
  }
  return host? host.value().to_string() : ascii_domain;
}

bool is_valid_port(const std::string &port) {
  auto first = begin(port), last = end(port);
  auto it = first;

  const char* port_first = std::addressof(*it);
  char* port_last = 0;
  auto value = std::strtoul(port_first, &port_last, 10);
  return (std::addressof(*last) == port_last) && (port_first != port_last) &&
      (value < std::numeric_limits<std::uint16_t>::max());
}

bool is_url_code_point(char c) {
  return
      std::isalnum(c, std::locale("C")) || is_in(c, "!$&'()*+,-./:/=?@_~");
}

bool is_windows_drive_letter(
    string_view::const_iterator it,
    string_view::const_iterator last) {
  if (std::distance(it, last) < 2) {
    return false;
  }

  if (!std::isalpha(*it, std::locale("C"))) {
    return false;
  }

  ++it;
  return ((*it == ':') || (*it == '|'));
}

inline bool is_windows_drive_letter(const std::string &segment) {
  auto view = string_view(segment);
  return is_windows_drive_letter(begin(view), end(view));
}

bool is_single_dot_path_segment(const std::string &segment) {
  auto segment_lower = segment;
  std::transform(begin(segment_lower), end(segment_lower), begin(segment_lower),
                 [] (char ch) -> char { return std::tolower(ch, std::locale("C")); });

  return ((segment_lower == ".") || (segment_lower == "%2e"));
}

bool is_double_dot_path_segment(const std::string &segment) {
  auto segment_lower = segment;
  std::transform(begin(segment_lower), end(segment_lower), begin(segment_lower),
                 [] (char ch) -> char { return std::tolower(ch, std::locale("C")); });

  return (
      (segment_lower == "..") ||
          (segment_lower == ".%2e") ||
          (segment_lower == "%2e.") ||
          (segment_lower == "%2e%2e"));
}

void shorten_path(const std::string &scheme, std::vector<std::string> &path) {
  if (path.empty()) {
    return;
  }

  if ((scheme.compare("file") == 0) &&
      (path.size() == 1) &&
      is_windows_drive_letter(path.front())) {
    return;
  }

  path.pop_back();
}
} // namespace

url_parser_context::url_parser_context(
    std::string input,
    const optional<url_record> &base,
    const optional<url_record> &url,
    optional<url_state> state_override)
    : input(input)
    , base(base)
    , url(url? url.value() : url_record{})
    , state(state_override? state_override.value() : url_state::scheme_start)
    , state_override(state_override)
    , buffer()
    , at_flag(false)
    , square_braces_flag(false)
    , password_token_seen_flag(false)
    , validation_error(false) {
  validation_error |= !remove_leading_whitespace(this->input);
  validation_error |= !remove_trailing_whitespace(this->input);
  validation_error |= !remove_tabs_and_newlines(this->input);

  view = string_view(this->input);
  it = begin(view);
}

url_parse_action url_parser_context::parse_scheme_start(char c) {
  if (std::isalpha(c, std::locale("C"))) {
    auto lower = std::tolower(c, std::locale("C"));
    buffer.push_back(lower);
    state = url_state::scheme;
  } else if (!state_override) {
    state = url_state::no_scheme;
    reset();
    return url_parse_action::continue_;
  } else {
    validation_error = true;
    return url_parse_action::fail;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_scheme(char c) {
  if (std::isalnum(c, std::locale("C")) || is_in(c, "+-.")) {
    auto lower = std::tolower(c, std::locale("C"));
    buffer.push_back(lower);
  } else if (c == ':') {
    if (state_override) {
      if (url.is_special() && !details::is_special(string_view(buffer))) {
        return url_parse_action::fail;
      }

      if (!url.is_special() && details::is_special(string_view(buffer))) {
        return url_parse_action::fail;
      }

      if ((url.includes_credentials() || url.port) &&(buffer.compare("file") == 0)) {
        return url_parse_action::fail;
      }

      if ((url.scheme.compare("file") == 0) && (!url.host || url.host.value().empty())) {
        return url_parse_action::fail;
      }
    }

    url.scheme = buffer;

    if (state_override) {
      // TODO: check default port
    }
    buffer.clear();

    if (url.scheme.compare("file") == 0) {
      if (!remaining_starts_with(it, end(view), "//")) {
        validation_error = true;
      }
      state = url_state::file;
    } else if (url.is_special() && base && (base.value().scheme == url.scheme)) {
      state = url_state::special_relative_or_authority;
    } else if (url.is_special()) {
      state = url_state::special_authority_slashes;
    } else if (remaining_starts_with(it, end(view), "/")) {
      state = url_state::path_or_authority;
      increment();
    } else {
      url.cannot_be_a_base_url = true;
      url.path.emplace_back();
      state = url_state::cannot_be_a_base_url_path;
    }
  } else if (!state_override) {
    buffer.clear();
    state = url_state::no_scheme;
    reset();
    return url_parse_action::continue_;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_no_scheme(char c) {
  if (!base || (base.value().cannot_be_a_base_url && (c != '#'))) {
    validation_error = true;
    return url_parse_action::fail;
  } else if (base.value().cannot_be_a_base_url && (c == '#')) {
    url.scheme = base.value().scheme;
    url.path = base.value().path;
    url.query = base.value().query;
    url.fragment = std::string();

    url.cannot_be_a_base_url = true;
    state = url_state::fragment;
  } else if (base.value().scheme.compare("file") != 0) {
    state = url_state::relative;
    reset();
    return url_parse_action::continue_;
  } else {
    state = url_state::file;
    reset();
    return url_parse_action::continue_;
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_special_relative_or_authority(char c) {
  if ((c == '/') && remaining_starts_with(it, end(view), "/")) {
    increment();
    state = url_state::special_authority_ignore_slashes;
  } else {
    validation_error = true;
    decrement();
    state = url_state::relative;
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_path_or_authority(char c) {
  if (c == '/') {
    state = url_state::authority;
  } else {
    state = url_state::path;
    decrement();
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_relative(char c) {
  url.scheme = base.value().scheme;
  if (is_eof()) {
    url.username = base.value().username;
    url.password = base.value().password;
    url.host = base.value().host;
    url.port = base.value().port;
    url.path = base.value().path;
    url.query = base.value().query;
  }
  else if (c == '/') {
    state = url_state::relative_slash;
  } else if (c == '?') {
    url.username = base.value().username;
    url.password = base.value().password;
    url.host = base.value().host;
    url.port = base.value().port;
    url.path = base.value().path;
    url.query = std::string();
    state = url_state::query;
  } else if (c == '#') {
    url.username = base.value().username;
    url.password = base.value().password;
    url.host = base.value().host;
    url.port = base.value().port;
    url.path = base.value().path;
    url.query = base.value().query;
    url.fragment = std::string();
    state = url_state::fragment;
  } else {
    if (url.is_special() && (c == '\\')) {
      validation_error = true;
      state = url_state::relative_slash;
    } else {
      url.username = base.value().username;
      url.password = base.value().password;
      url.host = base.value().host;
      url.port = base.value().port;
      url.path = base.value().path;
      if (!url.path.empty()) {
        url.path.pop_back();
      }
      state = url_state::path;
      decrement();
    }
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_relative_slash(char c) {
  if (url.is_special() && ((c == '/') || (c == '\\'))) {
    if (c == '\\') {
      validation_error = true;
    }
    state = url_state::special_authority_ignore_slashes;
  }
  else if (c == '/') {
    state = url_state::authority;
  }
  else {
    url.username = base.value().username;
    url.password = base.value().password;
    url.host = base.value().host;
    url.port = base.value().port;
    state = url_state::path;
    decrement();
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_special_authority_slashes(char c) {
  if ((c == '/') && remaining_starts_with(it, end(view), "/")) {
    increment();
    state = url_state::special_authority_ignore_slashes;
  } else {
    validation_error = true;
    decrement();
    state = url_state::special_authority_ignore_slashes;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_special_authority_ignore_slashes(char c) {
  if ((c != '/') && (c != '\\')) {
    decrement();
    state = url_state::authority;
  } else {
    validation_error = true;
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_authority(char c) {
  if (c == '@') {
    validation_error = true;
    if (at_flag) {
      buffer = "%40" + buffer;
    }
    at_flag = true;

    for (auto c : buffer) {
      if (c == ':' && !password_token_seen_flag) {
        password_token_seen_flag = true;
        continue;
      }

      auto pct_encoded = std::string();
      details::pct_encode_char(
          c, std::back_inserter(pct_encoded), " \"<>`#?{}/:;=@[\\]^|");

      if (password_token_seen_flag) {
        url.password += pct_encoded;
      } else {
        url.username += pct_encoded;
      }
    }
    buffer.clear();
  } else if (
      ((is_eof()) || (c == '/') || (c == '?') || (c == '#')) ||
          (url.is_special() && (c == '\\'))) {
    if (at_flag && buffer.empty()) {
      validation_error = true;
      return url_parse_action::fail;
    }
    restart_from_buffer();
    state = url_state::host;
    buffer.clear();
    return url_parse_action::increment;
  } else {
    buffer.push_back(c);
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_hostname(char c) {
  if (state_override && (url.scheme.compare("file") == 0)) {
    decrement();
    state = url_state::file_host;
  } else if ((c == ':') && !square_braces_flag) {
    if (buffer.empty()) {
      validation_error = true;
      return url_parse_action::fail;
    }

    auto host = parse_host(string_view(buffer), false);
    if (!host) {
      return url_parse_action::fail;
    }
    url.host = host.value();
    buffer.clear();
    state = url_state::port;

    if (state_override && (state_override.value() == url_state::hostname)) {
      return url_parse_action::success;
    }
  } else if (
      ((is_eof()) || (c == '/') || (c == '?') || (c == '#')) ||
          (url.is_special() && (c == '\\'))) {
    decrement();

    if (url.is_special() && buffer.empty()) {
      validation_error = true;
      return url_parse_action::fail;
    }

    auto host = parse_host(string_view(buffer), false);
    if (!host) {
      return url_parse_action::fail;
    }
    url.host = host.value();
    buffer.clear();
    state = url_state::path_start;
  } else {
    if (c == '[') {
      square_braces_flag = true;
    } else if (c == ']') {
      square_braces_flag = false;
    }
    buffer += c;
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_port(char c) {
  if (std::isdigit(c, std::locale("C"))) {
    buffer += c;
  } else if (
      ((is_eof()) || (c == '/') || (c == '?') || (c == '#')) ||
          (url.is_special() && (c == '\\')) ||
          state_override) {
    if (!buffer.empty()) {
      if (!is_valid_port(buffer)) {
        validation_error = true;
        return url_parse_action::fail;
      }

      auto view = string_view(url.scheme.data(), url.scheme.length());
      auto port = std::atoi(buffer.c_str());
      if (details::is_default_port(view, port)) {
        url.port = nullopt;
      }
      else {
        url.port = port;
      }
      buffer.clear();
    }

    if (state_override) {
      return url_parse_action::success;
    }

    decrement();
    state = url_state::path_start;
  } else {
    validation_error = true;
    return url_parse_action::fail;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_file(char c) {
  url.scheme = "file";

  if ((c == '/') || (c == '\\')) {
    if (c == '\\') {
      validation_error = true;
    }
    state = url_state::file_slash;
  } else if (base && (base.value().scheme.compare("file") == 0)) {
    if (is_eof()) {
      url.host = base.value().host;
      url.path = base.value().path;
      url.query = base.value().query;
    }
    else if (c == '?') {
      url.host = base.value().host;
      url.path = base.value().path;
      url.query = std::string();
      state = url_state::query;
    } else if (c == '#') {
      url.host = base.value().host;
      url.path = base.value().path;
      url.query = base.value().query;
      url.fragment = std::string();
      state = url_state::fragment;
    } else {
      if (!is_windows_drive_letter(it, end(view))) {
        url.host = base.value().host;
        url.path = base.value().path;
        shorten_path(url.scheme, url.path);
      }
      else {
        validation_error = true;
      }
      decrement();
      state = url_state::path;
    }
  } else {
    decrement();
    state = url_state::path;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_file_slash(char c) {
  if ((c == '/') || (c == '\\')) {
    if (c == '\\') {
      validation_error = true;
    }
    state = url_state::file_host;
  } else {
    if (base &&
            ((base.value().scheme.compare("file") == 0) && !is_windows_drive_letter(it, end(view)))) {
      if (!base.value().path.empty() && is_windows_drive_letter(base.value().path[0])) {
        url.path.push_back(base.value().path[0]);
      } else {
        url.host = base.value().host;
      }
    }

    state = url_state::path;
    decrement();
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_file_host(char c) {
  if ((is_eof()) || (c == '/') || (c == '\\') || (c == '?') || (c == '#')) {
    decrement();

    if (!state_override && is_windows_drive_letter(buffer)) {
      validation_error = true;
      state = url_state::path;
    } else if (buffer.empty()) {
      url.host = std::string();

      if (state_override) {
        return url_parse_action::success;
      }

      state = url_state::path_start;
    } else {
      auto host = parse_host(string_view(buffer), !url.is_special());
      if (!host) {
        return url_parse_action::fail;
      }

      if (host.value() == "localhost") {
        host.value().clear();
      }
      url.host = host.value();

      if (state_override) {
        return url_parse_action::success;
      }

      buffer.clear();

      state = url_state::path_start;
    }
  } else {
    buffer.push_back(c);
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_path_start(char c) {
  if (url.is_special()) {
    if (c == '\\') {
      validation_error = true;
    }
    state = url_state::path;
    if ((c != '/') && (c != '\\')) {
      decrement();
    }
  } else if (!state_override && (c == '?')) {
    url.query = std::string();
    state = url_state::query;
  } else if (!state_override && (c == '#')) {
    url.fragment = std::string();
    state = url_state::fragment;
  } else if (!is_eof()) {
    state = url_state::path;
    if (c != '/') {
      decrement();
    }
    else {
      url.path.push_back(buffer);
    }
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_path(char c) {
  if (((is_eof()) || (c == '/')) ||
      (url.is_special() && (c == '\\')) ||
      (!state_override && ((c == '?') || (c == '#')))) {
    if (url.is_special() && (c == '\\')) {
      validation_error = true;
    }

    if (is_double_dot_path_segment(buffer)) {
      shorten_path(url.scheme, url.path);
      if (!((c == '/') || (url.is_special() && (c == '\\')))) {
        url.path.emplace_back();
      }
    } else if (
        is_single_dot_path_segment(buffer) &&
            !((c == '/') || (url.is_special() && (c == '\\')))) {
      url.path.emplace_back();
    } else if (!is_single_dot_path_segment(buffer)) {
      if ((url.scheme.compare("file") == 0) && url.path.empty() && is_windows_drive_letter(buffer)) {
        if (!url.host || !url.host.value().empty()) {
          validation_error = true;
          url.host = std::string();
        }
        buffer[1] = ':';
      }

      url.path.push_back(buffer);
    }

    buffer.clear();

    if ((url.scheme.compare("file") == 0) && (is_eof() || (c == '?') || (c == '#'))) {
      auto path = std::deque<std::string>(begin(url.path), end(url.path));
      while ((path.size() > 1) && path[0].empty()) {
        validation_error = true;
        path.pop_front();
      }
      url.path.assign(begin(path), end(path));
    }

    if (c == '?') {
      url.query = std::string();
      state = url_state::query;
    }

    if (c == '#') {
      url.fragment = std::string();
      state = url_state::fragment;
    }
  } else {
    details::pct_encode_char(c, std::back_inserter(buffer), " \"<>`#?{}");
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_cannot_be_a_base_url(char c) {
  if (c == '?') {
    url.query = std::string();
    state = url_state::query;
  } else if (c == '#') {
    url.fragment = std::string();
    state = url_state::fragment;
  } else {
    if (!is_eof() && !is_url_code_point(c) && (c != '%')) {
      validation_error = true;
    }
    else if ((c == '%') && !details::is_pct_encoded(it, end(view), std::locale("C"))) {
      validation_error = true;
    }
    if (!is_eof()) {
      auto el = std::string();
      details::pct_encode_char(c, std::back_inserter(el));
      url.path[0] += el;
    }
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_query(char c) {
  if (!state_override && (c == '#')) {
    url.fragment = std::string();
    state = url_state::fragment;
  } else if (!is_eof()) {
    details::pct_encode_char(c, std::back_inserter(url.query.value()), " \"#<>");
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_fragment(char c) {
  if (c == '\0') {
    validation_error = true;
  } else {
    details::pct_encode_char(c, std::back_inserter(url.fragment.value()), " \"<>`");
  }
  return url_parse_action::increment;
}
}  // namespace skyr
