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
#include "skyr/url_parse.hpp"
#include "skyr/url/details/encode.hpp"
#include "skyr/url/details/decode.hpp"
#include "url_schemes.hpp"
#include "skyr/url_state.hpp"

namespace skyr {
namespace {
inline bool is_in(string_view::value_type c,
                  string_view view) {
  auto first = begin(view), last = end(view);
  return last != std::find(first, last, c);
}

inline bool is_in(string_view::value_type c,
                  const char *chars) {
  return is_in(c, string_view{chars});
}

bool remove_leading_whitespace(std::string &input) {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(first, last, [](char ch) -> bool {
    return
        !(std::isspace(ch, std::locale()) || is_in(ch, "\0\x1b\x04\x12\x1f"));
  });
  if (it != first) {
    input = std::string(it, last);
  }

  return it == first;
}

bool remove_trailing_whitespace(std::string &input) {
  using reverse_iterator = std::reverse_iterator<std::string::const_iterator>;

  auto first = reverse_iterator(end(input)),
      last = reverse_iterator(begin(input));
  auto it = std::find_if(first, last, [](char ch) -> bool {
    return
        !(std::isspace(ch, std::locale()) || is_in(ch, "\0\x1b\x04\x12\x1f"));
  });
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
      return false;
    }
  }
  return true;
}

bool parse_ipv6_address(
    const std::string &buffer,
    std::string &host) {
  auto address = std::string("0.0.0.0.0.0.0.0");
  auto piece_index = 0;
  auto compress = optional<decltype(piece_index)>();

  auto view = string_view(buffer);
  auto first = begin(view), last = end(view);
  auto it = first;
  if (*it == ':') {
    if (!remaining_starts_with(it, last, "/")) {
      // validation error
      return false;
    }
  }

  while (it != last) {
    if (piece_index == 8) {
      // validation error
      return false;
    }

    if (*it == ':') {
      if (compress) {
        // validation error
        return false;
      }

      ++it;
      ++piece_index;
      continue;
    }

    auto value = 0;
    auto length = 0;

    while ((length < 4) && std::isalnum(*it, std::locale("C"))) {
      value = value * 0x10 + *it;
      ++it;
      ++length;
    }

    if (*it == '.') {
      if (length == 0) {
        return false;
      }

      it -= length;

      if (piece_index > 6) {
        return false;
      }

      auto numbers_be_seen = 0;

      while (it != last) {
        auto ipv4_piece = optional<decltype(piece_index)>();

        if (numbers_be_seen > 0) {
          if ((*it == '.') && (numbers_be_seen < 4)) {
            ++it;
          }
          else {
            return false;
          }
        }

        if (!std::isalpha(*it, std::locale("C"))) {
          return false;
        }

        while (it != last) {
          auto number = *it - '0';
          if (!ipv4_piece) {
            ipv4_piece = number;
          }
          else if (ipv4_piece.value() == 0) {
            return false;
          }
          else {
            ipv4_piece = ipv4_piece.value() * 0x10 + number;
          }

          if (ipv4_piece > 255) {
            return false;
          }

          ++it;
        }

        address[piece_index] = address[piece_index] * 0x100 + ipv4_piece.value();
        ++numbers_be_seen;

        if ((numbers_be_seen == 2) || (numbers_be_seen == 4)) {
          ++piece_index;
        }
      }

      if (numbers_be_seen != 4) {
        return false;
      }

      break;
    }
    else if (*it == ':') {
      ++it;
      if (it == last) {
        return false;
      }
    }
    else if (it != last) {
      return false;
    }
    address[piece_index] = value;
    ++piece_index;
  }

  if (compress) {
    auto swaps = piece_index - compress.value();
    piece_index = 7;
    while ((piece_index != 0) && (swaps > 0)) {
      std::swap(address[piece_index], address[compress.value() + swaps - 1]);
      --piece_index;
      --swaps;
    }
  }
  else {
    if (!compress && (piece_index != 8)) {
      return false;
    }
  }

  host = address;

  return true;
}

bool parse_ipv4_number(
    const std::string &input,
    std::uint16_t &number,
    bool &validation_error_flag) {

//  auto R = 10;
//
//  auto first = begin(input), last = end(input);
//  auto it = first;
//
//  if (input.size() > 4) {
//    auto code_points = 0;
//    auto counter = 0;
//
//    while (it != last && counter < 2) {
//      auto view = string_view(std::addressof(*it), 2);
//      if ((view.compare("0x") == 0) ||
//          (view.compare("0X") == 0)) {
//        ++code_points;
//      }
//
//      it += 2;
//      ++counter;
//    }
//
//    auto input_2 = input;
//    if (code_points == 2) {
//      input_2 = std::string(it, last);
//    }
//    R = 16;
//  }



  if (input.empty()) {
    number = 0;
    return true;
  }

  // TODO: this totally sucks
  std::stringstream ss(input);
  ss >> number;
  return !!ss;
}

bool parse_ipv4_address(
    const std::string &buffer,
    std::string &host) {
  auto validation_error_flag = false;

  std::vector<std::string> parts;
  parts.push_back(std::string());
  for (const auto ch : buffer) {
    if (ch == '.') {
      parts.push_back(std::string());
    }
    else {
      parts.back().push_back(ch);
    }
  }

  if (parts.back().empty()) {
    validation_error_flag = true;
    if (parts.size() > 1) {
      parts.pop_back();
    }
  }

  if (parts.size() > 4) {
    host = buffer;
    return true;
  }

  auto numbers = std::vector<std::uint16_t>();

  for (const auto &part : parts) {
    if (part.empty()) {
      host = buffer;
      return true;
    }

    auto number = std::uint16_t(0);
    if (!parse_ipv4_number(part, number, validation_error_flag)) {
      host = buffer;
      return true;
    }

    numbers.push_back(number);
  }

  if (validation_error_flag) {
    return false;
  }

  auto numbers_first = begin(numbers), numbers_last = end(numbers);
  auto numbers_it = numbers_first;
  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  while (numbers_it != numbers_last_but_one) {
    if (*numbers_it > 255) {
      return false;
    }
    ++numbers_it;
  }

  if (numbers.back() > std::pow(256, 5 - numbers.size())) {
    return false;
  }

  numbers_it = numbers_first;
  while (true) {
    host += std::to_string(*numbers_it);
    ++numbers_it;
    if (numbers_it == numbers_last) {
      break;
    }
    else {
      host += ".";
    }
  }

  auto ipv4 = std::uint64_t(numbers.back());
  numbers.pop_back();

  for (auto i = 0UL; i < numbers.size(); ++i) {
    ipv4 += numbers[i] * std::pow(256, 3 - i);
  }

  return true;
}
//
//enum class processing_options {
//  transitional,
//  nontransational,
//};
//
//void domain_to_host(const std::string &domain, bool be_strict = false) {
//  auto check_hyphens = false;
//  auto check_bidi = false;
//  auto check_joiners = false;
//  auto use_std3_ascii_rules = false;
//  auto processing_option = processing_options::transitional;
//  bool verify_dns_length = false;
//
//
//}

bool parse_host(
    const std::string &buffer,
    std::string &host,
    bool is_not_special = false) {
  if (buffer.front() == '[') {
    if (buffer.back() != ']') {
      // result.validation_error = true;
      return false;
    }
    return parse_ipv6_address(buffer, host);
  }

  if (is_not_special) {
    auto it = std::find_if(
        begin(buffer), end(buffer),
        [] (char c) -> bool {
          static const char forbidden[] = "\0\t\n\r #/:?@[\\]";
          const char *first = forbidden, *last = forbidden + sizeof(forbidden);
          return last != std::find(first, last, c);
        });
    if (it != std::end(buffer)) {
      // result.validation_error = true;
      return false;
    }

    auto output = std::string();
    for (auto c : buffer) {
      details::encode_char(c, std::back_inserter(output));
    }

    host = output;
    return true;
  }

  auto domain = std::string{};
  try {
    details::decode(begin(buffer), end(buffer), std::back_inserter(domain));
  }
  catch (percent_decoding_error &) {
    // result.validation_error = true;
    return false;
  }
  auto it = std::find_if(begin(domain), end(domain), is_forbidden_host_point);
  if (it != end(domain)) {
    // result.validation_error = true;
    return false;
  }

  auto ipv4_host = std::string();
  if (parse_ipv4_address(domain, ipv4_host)) {
    host = ipv4_host;
  }
  else {
    host = domain;
  }
  return true;
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

//bool is_surrogate(char c) {
//  return ((static_cast<int>(c) >= 0xd800) && static_cast<int>(c) <= 0xdfff);
//}
//
//bool is_scalar(char c) {
//  return !is_surrogate(c);
//}

//bool is_ascii_hex_digit(char c) {
//  return
//      ((static_cast<int>(c) >= 0x41) && static_cast<int>(c) <= 0x46) ||
//      ((static_cast<int>(c) >= 0x61) && static_cast<int>(c) <= 0x66)
//          ;
//}

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

  return ((segment == ".") || (segment == "%2e"));
}

bool is_pct_encoded(string_view::const_iterator it,
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

void shorten_path(url_record &url) {
  auto &path = url.path;

  if (path.empty()) {
    return;
  }

  if ((url.scheme.compare("file") == 0) &&
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
  validation_error |= !remove_leading_whitespace(input);
  validation_error |= !remove_trailing_whitespace(input);
  validation_error |= !remove_tabs_and_newlines(input);

  view = string_view(this->input);
  first = begin(view), last = end(view);
  it = first;
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
      if (!remaining_starts_with(it, last, "//")) {
        validation_error = true;
      }
      state = url_state::file;
    } else if (url.is_special() && base && (base.value().scheme == url.scheme)) {
      state = url_state::special_relative_or_authority;
    } else if (url.is_special()) {
      state = url_state::special_authority_slashes;
    } else if (remaining_starts_with(it, last, "/")) {
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
  if ((c == '/') && remaining_starts_with(it, last, "/")) {
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

    // TODO: check that this isn't a temporary fix for e.g. mailto:/example.com/
    if (c == '/') {
      decrement();
    }
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
  if (is_eof()) {
    url.username = base.value().username;
    url.password = base.value().password;
    url.host = base.value().host;
    url.port = base.value().port;
    state = url_state::path;
    decrement();
  }
  else if (url.is_special() && ((c == '/') || (c == '\\'))) {
    if (c == '\\') {
      validation_error = true;
      state = url_state::special_authority_ignore_slashes;
    }
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
  if ((c == '/') && remaining_starts_with(it, last, "/")) {
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

      // TODO: percent encode

      if (password_token_seen_flag) {
        url.password += c;
      } else {
        url.username += c;
      }
    }
    buffer.clear();

    state = url_state::host;
    increment();
    return url_parse_action::continue_;
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
    return url_parse_action::continue_;
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

    auto host = std::string();
    if (!parse_host(buffer, host, false)) {
      return url_parse_action::fail;
    }
    url.host = host;
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

    auto host = std::string();
    if (!parse_host(buffer, host, false)) {
      return url_parse_action::fail;
    }
    url.host = host;
    buffer.clear();
    state = url_state::path;
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
      if (!is_windows_drive_letter(it, last)) {
        url.host = base.value().host;
        url.path = base.value().path;
        shorten_path(url);
      }
    }
  } else {
    decrement();
    state = url_state::path;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_file_slash(char c) {
  if (is_eof()) {
    decrement();
    state = url_state::path;
  }
  else if ((c == '/') || (c == '\\')) {
    if (c == '\\') {
      validation_error = true;
    }
    state = url_state::file_host;
  } else if (
      base &&
          ((base.value().scheme.compare("file") == 0) && !is_windows_drive_letter(it, last))) {
    if (!base.value().path.empty() && is_windows_drive_letter(base.value().path[0])) {
      url.path.push_back(base.value().path[0]);
    } else {
      url.host = base.value().host;
    }

    decrement();
    state = url_state::path;
  }

  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_file_host(char c) {
  if ((is_eof()) || (c == '/') || (c == '\\') || (c == '?') || (c == '#')) {
    decrement();

    if (!state_override && (is_windows_drive_letter(buffer))) {
      validation_error = true;
      state = url_state::path;
    } else if (buffer.empty()) {
      url.host = std::string();

      if (state_override) {
        return url_parse_action::success;
      }

      state = url_state::path_start;
    } else {
      auto host = std::string();
      if (!parse_host(buffer, host, url.is_special())) {
        return url_parse_action::fail;
      }

      if (host == "localhost") {
        host.clear();
      }
      url.host = host;

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
  } else {
    state = url_state::path;
    if (c != '/') {
      decrement();
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
    } else if (is_double_dot_path_segment(buffer)) {
      shorten_path(url);
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
      } else {
        url.path.push_back(buffer);
      }
    }

    buffer.clear();

    if ((url.scheme.compare("file") == 0) && ((it == last) || (c == '?') || (c == '#'))) {
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
    details::encode_char(c, std::back_inserter(buffer), " \"<>`#?{}");
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
    else if ((c == '%') && !is_pct_encoded(it, last)) {
      validation_error = true;
    }
    if (!is_eof()) {
      auto el = std::string();
      details::encode_char(c, std::back_inserter(el));
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
    details::encode_char(c, std::back_inserter(url.query.value()), " \"#<>`");
  }
  return url_parse_action::increment;
}

url_parse_action url_parser_context::parse_fragment(char c) {
  if (c == '\0') {
    validation_error = true;
  } else {
    details::encode_char(c, std::back_inserter(url.fragment.value()), " \"<>`");
  }
  return url_parse_action::increment;
}
}  // namespace skyr
