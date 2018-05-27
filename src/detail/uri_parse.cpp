// Copyright 2016-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <cmath>
#include <sstream>
#include <arpa/inet.h>
#include "network/uri/detail/uri_parse.hpp"
#include "network/uri/detail/encode.hpp"
#include "grammar.hpp"
#include "detail/url_schemes.hpp"

#include <iostream>

namespace network {
namespace detail {
namespace {
void remove_leading_whitespace(std::string &input, url_record &result) {
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

void remove_trailing_whitespace(std::string &input, url_record &result) {
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

void remove_tabs_and_newlines(std::string &input, url_record &result) {
  auto it = std::remove_if(begin(input), end(input),
                           [] (char c) -> bool { return (c == '\t') || (c == '\n'); });
  result.validation_error = (it != end(input));
  input.erase(it, end(input));
}

inline bool is_special_scheme(const std::string &scheme) {
  return is_special(string_view(scheme.data(), scheme.length()));
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

//bool validate_domain(const std::string &buffer) {
//  auto view = string_view(buffer);
//  auto first = begin(view), last = end(view);
//
//  auto it = first;
//  while (it != last) {
//    if (!is_unreserved(it, last) &&
//        !is_pct_encoded(it, last) &&
//        !is_sub_delim(it, last)) {
//      return false;
//    }
//  }
//  return true;
//}
//
//bool validate_ip_address(string_view::const_iterator first,
//                         string_view::const_iterator last,
//                         int family) {
//  // the maximum ipv6 address length is 45 - choose a buffer on the
//  // stack large enough
//  char addr[64];
//  std::memset(addr, 0, sizeof(addr));
//  std::copy(first, last, std::begin(addr));
//  string_view::value_type buffer[sizeof(struct in6_addr)];
//  int rc = ::inet_pton(family, addr, &buffer);
//  return rc > 0;
//}
//
//inline bool validate_ipv4_address(const std::string &buffer) {
//  auto view = string_view(buffer);
//  auto first = begin(view), last = end(view);
//  return validate_ip_address(first, last, AF_INET);
//}

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

bool parse_host(
    const std::string &scheme,
    const std::string &buffer,
    std::string &host) {
  if (buffer.front() == '[') {
    if (buffer.back() != ']') {
      // result.validation_error = true;
      return false;
    }

    if (!parse_ipv6_address(buffer, host)) {
      return false;
    }
  }
  else {
    auto domain = std::string();
    encode_host(begin(buffer), end(buffer), std::back_inserter(domain));
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

inline bool is_windows_drive_letter(const std::string &el) {
  auto view = string_view(el);
  return is_windows_drive_letter(begin(view), end(view));
}

void shorten_path(std::vector<std::string> &path, url_record &result) {
  if (path.empty()) {
    return;
  }

  if ((result.scheme.compare("file") == 0) && (path.size() == 1) && is_windows_drive_letter(path.front())) {
    return;
  }

  path.pop_back();
}
} // namespace

url_record basic_parse(
    std::string input,
    const optional<url_record> &base,
    const optional<url_record> &url,
    url_state state_override) {
  auto result = url? url.value() : url_record{};
  result.url = input;


  if (input == "/..//localhost//pig") {
    std::cout << "URL:  " << input << std::endl;
    if (base) {
      std::cout << "BASE: " << base.value().url << std::endl;
    }
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

  while (true) {
    if (state == url_state::scheme_start) {
      if (std::isalpha(*it, std::locale("C"))) {
        auto lower = std::tolower(*it);
        buffer.push_back(static_cast<char>(lower));
        state = url_state::scheme;
      } else if (state_override == url_state::null) {
        state = url_state::no_scheme;
        it = first;
        continue;
      } else {
        result.validation_error = true;
        return result;
      }
    } else if (state == url_state::scheme) {
      if (std::isalnum(*it, std::locale("C")) || is_in(*it, "+-.")) {
        auto lower = std::tolower(*it, std::locale("C"));
        buffer.push_back(lower);
      } else if (*it == ':') {
        if (state_override != url_state::null) {
          // TODO
        }

        result.scheme = buffer;
        buffer.clear();

        if (result.scheme.compare("file") == 0) {
          auto next = it;
          ++next;
          if (!is_double_slash(next, last)) {
            result.validation_error = true;
          }
          state = url_state::file;
        } else if (is_special_scheme(result.scheme) && base && (base.value().scheme == result.scheme)) {
          state = url_state::special_relative_or_authority;
        } else if (is_special_scheme(result.scheme)) {
          state = url_state::special_authority_slashes;
        } else if (remaining_starts_with(it, last, "/")) {
          state = url_state::path_or_authority;
          ++it;
        } else {
          result.cannot_be_a_base_url = true;
          result.path.push_back(std::string());
          state = url_state::cannot_be_a_base_url_path;
        }
      } else if (state_override == url_state::null) {
        buffer.clear();
        state = url_state::no_scheme;
        it = first;
        continue;
      }
    } else if (state == url_state::no_scheme) {
      if (!base || (base.value().cannot_be_a_base_url && (*it != '#'))) {
        result.validation_error = true;
        return result;
      } else if (base.value().cannot_be_a_base_url && (*it == '#')) {
        result.scheme = base.value().scheme;
        result.path = base.value().path;
        result.query = base.value().query;
        result.fragment = std::string();

        result.cannot_be_a_base_url = true;
        state = url_state::fragment;
      } else if (base.value().scheme.compare("file") != 0) {
        state = url_state::relative;
        it = first;
        continue;
      } else {
        state = url_state::file;
        it = first;
        continue;
      }
    } else if (state == url_state::special_relative_or_authority) {
      if ((*it == '/') && remaining_starts_with(it, last, "/")) {
        ++it;
        state = url_state::special_authority_ignore_slashes;
      } else {
        result.validation_error = true;
        --it;
        state = url_state::relative;
      }
    } else if (state == url_state::path_or_authority) {
      if (*it == '/') {
        state = url_state::authority;
      } else {
        state = url_state::path;
        --it;
      }
    } else if (state == url_state::relative) {
      result.scheme = base.value().scheme;
      if (it == last) {
        result.username = base.value().username;
        result.password = base.value().password;
        result.host = base.value().host;
        result.port = base.value().port;
        result.path = base.value().path;
        result.query = base.value().query;
      }
      else if (*it == '/') {
        state = url_state::relative_slash;
      } else if (*it == '?') {
        result.username = base.value().username;
        result.password = base.value().password;
        result.host = base.value().host;
        result.port = base.value().port;
        result.path = base.value().path;
        result.query = std::string();
        state = url_state::query;
      } else if (*it == '#') {
        result.username = base.value().username;
        result.password = base.value().password;
        result.host = base.value().host;
        result.port = base.value().port;
        result.path = base.value().path;
        result.query = base.value().query;
        result.fragment = std::string();
        state = url_state::fragment;
      } else {
        if (is_special_scheme(result.scheme) && (*it == '\\')) {
          result.validation_error = true;
          state = url_state::relative_slash;
        } else {
          result.username = base.value().username;
          result.password = base.value().password;
          result.host = base.value().host;
          result.port = base.value().port;
          result.path = base.value().path;
          if (!result.path.empty()) {
            result.path.pop_back();
          }
          state = url_state::path;
          --it;
        }
      }
    } else if (state == url_state::relative_slash) {
      if (it == last) {
        result.username = base.value().username;
        result.password = base.value().password;
        result.host = base.value().host;
        result.port = base.value().port;
        state = url_state::path;
        --it;
      }
      else if (is_special_scheme(result.scheme) && ((*it == '/') || (*it == '\\'))) {
        if (*it == '\\') {
          result.validation_error = true;
          state = url_state::special_authority_ignore_slashes;
        }
      }
      else if (*it == '/') {
        state = url_state::authority;
      }
      else {
        result.username = base.value().username;
        result.password = base.value().password;
        result.host = base.value().host;
        result.port = base.value().port;
        state = url_state::path;
        --it;
      }
    } else if (state == url_state::special_authority_slashes) {
      if ((*it == '/') && remaining_starts_with(it, last, "/")) {
        ++it;
        state = url_state::special_authority_ignore_slashes;
      } else {
        result.validation_error = true;
        --it;
        state = url_state::special_authority_ignore_slashes;
      }
    } else if (state == url_state::special_authority_ignore_slashes) {
      if ((*it != '/') && (*it != '\\')) {
        --it;
        state = url_state::authority;
      } else {
        result.validation_error = true;
      }
    } else if (state == url_state::authority) {
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
          } else {
            result.username += c;
          }
        }
        buffer.clear();

        state = url_state::host;
        ++it;
        continue;
      } else if ((it == last) || (*it == '/') || (*it == '?') || (*it == '#') ||
          (is_special_scheme(result.scheme) && (*it == '\\'))) {
        if (at_flag && buffer.empty()) {
          result.validation_error = true;
          return result;
        }
        it = it - buffer.size();
        state = url_state::host;
        buffer.clear();
        continue;
      } else {
        buffer.push_back(*it);
      }
    } else if (state == url_state::host) {
      if ((state_override != url_state::null) && (result.scheme.compare("file") == 0)) {
        --it;
        state = url_state::file_host;
      } else if ((*it == ':') && !square_brace_flag) {
        if (buffer.empty()) {
          result.validation_error = true;
          return result;
        }

        auto host = std::string();
        if (!parse_host(result.scheme, buffer, host)) {
          return result;
        }
        result.host = host;
        buffer.clear();
        state = url_state::port;

        if (state_override == url_state::hostname) {
          result.success = true;
          return result;
        }
      } else if ((it == last) || (*it == '/') || (*it == '?') || (*it == '#') ||
          (is_special_scheme(result.scheme) && (*it == '\\'))) {
        --it;

        if (is_special_scheme(result.scheme) && buffer.empty()) {
          result.validation_error = true;
          return result;
        }

        auto host = std::string();
        if (!parse_host(result.scheme, buffer, host)) {
          return result;
        }
        result.host = host;
        buffer.clear();
        state = url_state::path;
      } else {
        if (*it == '[') {
          square_brace_flag = true;
        } else if (*it == ']') {
          square_brace_flag = false;
        }
        buffer += *it;
      }
    } else if (state == url_state::port) {
      if (std::isdigit(*it, std::locale("C"))) {
        buffer += *it;
      } else if ((it == last) || (*it == '/') || (*it == '?') || (*it == '#') ||
          (is_special_scheme(result.scheme) && (*it == '\\')) ||
          (state_override != url_state::null)) {
        if (!buffer.empty()) {
          if (!is_valid_port(buffer)) {
            result.validation_error = true;
            return result;
          }

          auto view = string_view(result.scheme.data(), result.scheme.length());
          auto port = std::atoi(buffer.c_str());
          if (is_default_port(view, port)) {
            result.port = nullopt;
          }
          else {
            result.port = port;
          }
          buffer.clear();
        }

        if (state_override != url_state::null) {
          result.success = true;
          return result;
        }

        --it;
        state = url_state::path_start;
      } else {
        result.validation_error = true;
        return result;
      }
    } else if (state == url_state::file) {
      result.scheme = "file";

      if ((*it == '/') || (*it == '\\')) {
        if (*it == '\\') {
          result.validation_error = true;
        }
        state = url_state::file_slash;
      } else if (base && (base.value().scheme.compare("file") == 0)) {
        if (it == last) {
          result.host = base.value().host;
          result.path = base.value().path;
          result.query = base.value().query;
        }
        else if (*it == '?') {
          result.host = base.value().host;
          result.path = base.value().path;
          result.query = std::string();
          state = url_state::query;
        } else if (*it == '#') {
          result.host = base.value().host;
          result.path = base.value().path;
          result.query = base.value().query;
          result.fragment = std::string();
          state = url_state::fragment;
        } else {
          if (!is_windows_drive_letter(it, last)) {
            result.host = base.value().host;
            result.path = base.value().path;
            shorten_path(result.path, result);
          }
        }
      } else {
        --it;
        state = url_state::path;
      }
    } else if (state == url_state::file_slash) {
      if (it == last) {
        --it;
        state = url_state::path;
      }
      else if ((*it == '/') || (*it == '\\')) {
        if (*it == '\\') {
          result.validation_error = true;
        }
        state = url_state::file_host;
      } else if (base && ((base.value().scheme.compare("file") == 0) && !is_windows_drive_letter(it, last))) {
        if (!base.value().path.empty() && is_windows_drive_letter(base.value().path[0])) {
          result.path.push_back(base.value().path[0]);
        } else {
          result.host = base.value().host;
        }

        --it;
        state = url_state::path;
      }
    } else if (state == url_state::file_host) {
      if ((it == last) || (*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#')) {
        --it;

        if ((state_override == url_state::null) && (is_windows_drive_letter(buffer))) {
          result.validation_error = true;
          state = url_state::path;
        } else if (buffer.empty()) {
          result.host = std::string();

          if (state_override != url_state::null) {
            result.success = true;
            return result;
          }

          state = url_state::path_start;
        } else {
          auto host = std::string();
          if (!parse_host(result.scheme, buffer, host)) {
            return result;
          }

          if (host == "localhost") {
            host.clear();
          }
          result.host = host;

          if (state_override != url_state::null) {
            result.success = true;
            return result;
          }

          buffer.clear();

          state = url_state::path_start;
        }
      } else {
        buffer.push_back(*it);
      }
    } else if (state == url_state::path_start) {
      if (is_special_scheme(result.scheme)) {
        if (*it == '\\') {
          result.validation_error = true;
        }
        state = url_state::path;
      } else if (state_override == url_state::null) {
        if (*it == '?') {
          result.query = std::string();
          state = url_state::query;
        } else if (*it == '#') {
          result.fragment = std::string();
          state = url_state::fragment;
        }
      } else {
        state = url_state::path;
        if (*it != '/') {
          --it;
        }
      }
    } else if (state == url_state::path) {
      if ((*it == '/') ||
          (is_special_scheme(result.scheme) && (*it == '\\')) ||
          ((state_override == url_state::null) && ((*it == '?') || (*it == '#')))) {
        if (is_special_scheme(result.scheme) && (*it == '\\')) {
          result.validation_error = true;
        } else if (buffer == "..") {
          shorten_path(result.path, result);
          if ((*it != '/') && (is_special_scheme(result.scheme) && (*it == '\\'))) {
            result.path.push_back(std::string());
          }
        } else if ((buffer == ".") && ((*it != '/') && (is_special_scheme(result.scheme) && (*it == '\\')))) {
          result.path.push_back(std::string());
        } else if (buffer != ".") {
          if ((result.scheme.compare("file") == 0) && result.path.empty() && is_windows_drive_letter(buffer)) {
            if (!result.host || !result.host.value().empty()) {
              result.validation_error = true;
              result.host = std::string();
            }
            buffer[1] = ':';
          } else {
            result.path.push_back(buffer);
          }
        }

        buffer.clear();

        if (*it == '?') {
          result.query = std::string();
          state = url_state::query;
        }

        if (*it == '#') {
          result.fragment = std::string();
          state = url_state::fragment;
        }
      } else {
        buffer += *it;
      }
    } else if (state == url_state::cannot_be_a_base_url_path) {
      if (*it == '?') {
        result.query = std::string();
        state = url_state::query;
      } else if (*it == '#') {
        result.fragment = std::string();
        state = url_state::fragment;
      } else if (*it != '%') {
        result.validation_error = true;
      }
//      else {
//        result.path[0].push_back(*it);
//      }
    } else if (state == url_state::query) {
      if ((it == last) || (*it == '#')) {
        for (auto && ch : buffer) {
          encode_char(ch, std::back_inserter(*result.query), "/.@&%;=");
        }
        buffer.clear();

        if (*it == '#') {
          result.fragment = std::string();
          state = url_state::fragment;
        }
      } else {
        buffer += *it;
      }
    } else if (state == url_state::fragment) {
      if (*it == '\0') {
        result.validation_error = true;
      } else {
        encode_char(*it, std::back_inserter(*result.fragment), "/.@&l;=%");
      }
    }

    if (it == last) {
      break;
    }

    ++it;
  }


  result.success = true;
  return result;
}

url_record parse(
    std::string input,
    const optional<url_record> &base) {
  auto result = basic_parse(input, base);

  if (!result.success) {
    return result;
  }

  if (result.scheme.compare("blob") != 0) {
    return result;
  }

  if (result.path.empty()) {
    return result;
  }

  // TODO: check Blob URL store

  return result;
}
}  // namespace detail
}  // namespace network
