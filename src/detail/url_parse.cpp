// Copyright 2016-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <cmath>
#include <sstream>
#include <deque>
#include "skyr/url/url_parse.hpp"
#include "skyr/url/detail/encode.hpp"
#include "skyr/url/detail/decode.hpp"
#include "grammar.hpp"
#include "detail/url_schemes.hpp"

#include <iostream>

namespace skyr {
namespace {
void remove_leading_whitespace(std::string &input, url_record &url) {
  auto first = begin(input), last = end(input);
  auto it = std::find_if(first, last, [](char ch) -> bool {
    return
        !(std::isspace(ch, std::locale()) ||
        ch == '\0' || ch == '\x1b' || ch == '\x04' || ch == '\x12' || ch == '\x1f');
  });
  if (it != first) {
    url.validation_error = true;
    input = std::string(it, last);
  }
}

void remove_trailing_whitespace(std::string &input, url_record &url) {
  using reverse_iterator = std::reverse_iterator<std::string::const_iterator>;

  auto first = reverse_iterator(end(input)),
      last = reverse_iterator(begin(input));
  auto it = std::find_if(first, last, [](char ch) -> bool {
    return
        !(std::isspace(ch, std::locale()) ||
        ch == '\0' || ch == '\x1b' || ch == '\x04' || ch == '\x12' || ch == '\x1f');
  });
  if (it != first) {
    url.validation_error = true;
    input = std::string(it, last);
    std::reverse(begin(input), end(input));
  }
}

void remove_tabs_and_newlines(std::string &input, url_record &url) {
  auto it = std::remove_if(begin(input), end(input),
                           [] (char c) -> bool { return (c == '\t') || (c == '\n'); });
  url.validation_error = (it != end(input));
  input.erase(it, end(input));
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
      detail::encode_char(c, std::back_inserter(output));
    }

    host = output;
    return true;
  }

  auto domain = std::string{};
  try {
    detail::decode(begin(buffer), end(buffer), std::back_inserter(domain));
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
      std::isalnum(c, std::locale("C")) ||
      detail::is_in(c, "!$&'()*+,-./:/=?@_~")
          ;
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
  if (segment == ".") {
    return true;
  }
  else if ((segment == "%2e") || (segment == "%2E")) {
    return true;
  }
  return false;
}

bool is_double_dot_path_segment(const std::string &segment) {
  if (segment == "..") {
    return true;
  }
  else if ((segment == ".%2e") || (segment == ".%2E")) {
    return true;
  }
  else if ((segment == "%2e.") || (segment == "%2E.")) {
    return true;
  }
  else if ((segment == "%2e%2e") || (segment == "%2E%2e") || (segment == "%2e%2E") || (segment == "%2E%2E")) {
    return true;
  }
  return false;
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

bool url_record::is_special() const {
  return detail::is_special(string_view(scheme.data(), scheme.length()));
}

url_record basic_parse(
    std::string input,
    const optional<url_record> &base,
    const optional<url_record> &url_,
    optional<url_state> state_override) {
  auto url = url_? url_.value() : url_record{};
  url.url = input;


  if (input == "madeupscheme:/example.com/") {
    std::cout << "URL:  " << input << std::endl;
    if (base) {
      std::cout << "BASE: " << base.value().url << std::endl;
    }
  }

  if (input.empty()) {
    return url;
  }

  remove_leading_whitespace(input, url);
  remove_trailing_whitespace(input, url);
  remove_tabs_and_newlines(input, url);

  auto state = state_override? state_override.value() : url_state::scheme_start;

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
        buffer.push_back(std::tolower(*it, std::locale("C")));
        state = url_state::scheme;
      } else if (!state_override) {
        state = url_state::no_scheme;
        it = first;
        continue;
      } else {
        url.validation_error = true;
        return url;
      }
    } else if (state == url_state::scheme) {
      if (std::isalnum(*it, std::locale("C")) || detail::is_in(*it, "+-.")) {
        auto lower = std::tolower(*it, std::locale("C"));
        buffer.push_back(lower);
      } else if (*it == ':') {
        if (state_override) {
          // TODO
        }

        url.scheme = buffer;
//        if (url && url.value().port) {
//          auto port = url.value().port.value();
//          if (default_port(result.scheme).value() == port) {
//            return result;
//          }
//        }
        buffer.clear();

        if (url.scheme.compare("file") == 0) {
          if (!remaining_starts_with(it, last, "//")) {
            url.validation_error = true;
          }
//          auto next = it;
//          ++next;
//          if (!is_double_slash(next, last)) {
//            result.validation_error = true;
//          }
          state = url_state::file;
        } else if (url.is_special() && base && (base.value().scheme == url.scheme)) {
          state = url_state::special_relative_or_authority;
        } else if (url.is_special()) {
          state = url_state::special_authority_slashes;
        } else if (remaining_starts_with(it, last, "/")) {
          state = url_state::path_or_authority;
          ++it;
        } else {
          url.cannot_be_a_base_url = true;
          url.path.emplace_back();
          state = url_state::cannot_be_a_base_url_path;
        }
      } else if (!state_override) {
        buffer.clear();
        state = url_state::no_scheme;
        it = first;
        continue;
      }
    } else if (state == url_state::no_scheme) {
      if (!base || (base.value().cannot_be_a_base_url && (*it != '#'))) {
        url.validation_error = true;
        return url;
      } else if (base.value().cannot_be_a_base_url && (*it == '#')) {
        url.scheme = base.value().scheme;
        url.path = base.value().path;
        url.query = base.value().query;
        url.fragment = std::string();

        url.cannot_be_a_base_url = true;
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
        url.validation_error = true;
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
      url.scheme = base.value().scheme;
      if (it == last) {
        url.username = base.value().username;
        url.password = base.value().password;
        url.host = base.value().host;
        url.port = base.value().port;
        url.path = base.value().path;
        url.query = base.value().query;
      }
      else if (*it == '/') {
        state = url_state::relative_slash;
      } else if (*it == '?') {
        url.username = base.value().username;
        url.password = base.value().password;
        url.host = base.value().host;
        url.port = base.value().port;
        url.path = base.value().path;
        url.query = std::string();
        state = url_state::query;
      } else if (*it == '#') {
        url.username = base.value().username;
        url.password = base.value().password;
        url.host = base.value().host;
        url.port = base.value().port;
        url.path = base.value().path;
        url.query = base.value().query;
        url.fragment = std::string();
        state = url_state::fragment;
      } else {
        if (url.is_special() && (*it == '\\')) {
          url.validation_error = true;
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
          --it;
        }
      }
    } else if (state == url_state::relative_slash) {
      if (it == last) {
        url.username = base.value().username;
        url.password = base.value().password;
        url.host = base.value().host;
        url.port = base.value().port;
        state = url_state::path;
        --it;
      }
      else if (url.is_special() && ((*it == '/') || (*it == '\\'))) {
        if (*it == '\\') {
          url.validation_error = true;
          state = url_state::special_authority_ignore_slashes;
        }
      }
      else if (*it == '/') {
        state = url_state::authority;
      }
      else {
        url.username = base.value().username;
        url.password = base.value().password;
        url.host = base.value().host;
        url.port = base.value().port;
        state = url_state::path;
        --it;
      }
    } else if (state == url_state::special_authority_slashes) {
      if ((*it == '/') && remaining_starts_with(it, last, "/")) {
        ++it;
        state = url_state::special_authority_ignore_slashes;
      } else {
        url.validation_error = true;
        --it;
        state = url_state::special_authority_ignore_slashes;
      }
    } else if (state == url_state::special_authority_ignore_slashes) {
      if ((*it != '/') && (*it != '\\')) {
        --it;
        state = url_state::authority;
      } else {
        url.validation_error = true;
      }
    } else if (state == url_state::authority) {
      if (*it == '@') {
        url.validation_error = true;
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
        ++it;
        continue;
      } else if (
          ((it == last) || (*it == '/') || (*it == '?') || (*it == '#')) ||
          (url.is_special() && (*it == '\\'))) {
        if (at_flag && buffer.empty()) {
          url.validation_error = true;
          return url;
        }
        it = it - buffer.size();
        state = url_state::host;
        buffer.clear();
        continue;
      } else {
        buffer.push_back(*it);
      }
    } else if (state == url_state::host) {
      if (state_override && (url.scheme.compare("file") == 0)) {
        --it;
        state = url_state::file_host;
      } else if ((*it == ':') && !square_brace_flag) {
        if (buffer.empty()) {
          url.validation_error = true;
          return url;
        }

        auto host = std::string();
        if (!parse_host(buffer, host, false)) {
          return url;
        }
        url.host = host;
        buffer.clear();
        state = url_state::port;

        if (state_override && (state_override.value() == url_state::hostname)) {
          url.success = true;
          return url;
        }
      } else if (
          ((it == last) || (*it == '/') || (*it == '?') || (*it == '#')) ||
          (url.is_special() && (*it == '\\'))) {
        --it;

        if (url.is_special() && buffer.empty()) {
          url.validation_error = true;
          return url;
        }

        auto host = std::string();
        if (!parse_host(buffer, host, false)) {
          return url;
        }
        url.host = host;
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
      } else if (
          ((it == last) || (*it == '/') || (*it == '?') || (*it == '#')) ||
          (url.is_special() && (*it == '\\')) ||
          state_override) {
        if (!buffer.empty()) {
          if (!detail::is_valid_port(buffer)) {
            url.validation_error = true;
            return url;
          }

          auto view = string_view(url.scheme.data(), url.scheme.length());
          auto port = std::atoi(buffer.c_str());
          if (detail::is_default_port(view, port)) {
            url.port = nullopt;
          }
          else {
            url.port = port;
          }
          buffer.clear();
        }

        if (state_override) {
          url.success = true;
          return url;
        }

        --it;
        state = url_state::path_start;
      } else {
        url.validation_error = true;
        return url;
      }
    } else if (state == url_state::file) {
      url.scheme = "file";

      if ((*it == '/') || (*it == '\\')) {
        if (*it == '\\') {
          url.validation_error = true;
        }
        state = url_state::file_slash;
      } else if (base && (base.value().scheme.compare("file") == 0)) {
        if (it == last) {
          url.host = base.value().host;
          url.path = base.value().path;
          url.query = base.value().query;
        }
        else if (*it == '?') {
          url.host = base.value().host;
          url.path = base.value().path;
          url.query = std::string();
          state = url_state::query;
        } else if (*it == '#') {
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
          url.validation_error = true;
        }
        state = url_state::file_host;
      } else if (base && ((base.value().scheme.compare("file") == 0) && !is_windows_drive_letter(it, last))) {
        if (!base.value().path.empty() && is_windows_drive_letter(base.value().path[0])) {
          url.path.push_back(base.value().path[0]);
        } else {
          url.host = base.value().host;
        }

        --it;
        state = url_state::path;
      }
    } else if (state == url_state::file_host) {
      if ((it == last) || (*it == '/') || (*it == '\\') || (*it == '?') || (*it == '#')) {
        --it;

        if (!state_override && (is_windows_drive_letter(buffer))) {
          url.validation_error = true;
          state = url_state::path;
        } else if (buffer.empty()) {
          url.host = std::string();

          if (state_override) {
            url.success = true;
            return url;
          }

          state = url_state::path_start;
        } else {
          auto host = std::string();
          if (!parse_host(buffer, host, url.is_special())) {
            return url;
          }

          if (host == "localhost") {
            host.clear();
          }
          url.host = host;

          if (state_override) {
            url.success = true;
            return url;
          }

          buffer.clear();

          state = url_state::path_start;
        }
      } else {
        buffer.push_back(*it);
      }
    } else if (state == url_state::path_start) {
      if (url.is_special()) {
        if (*it == '\\') {
          url.validation_error = true;
        }
        state = url_state::path;
        if ((*it != '/') && (*it != '\\')) {
          --it;
        }
      } else if (!state_override && (*it == '?')) {
        url.query = std::string();
        state = url_state::query;
      } else if (!state_override && (*it == '#')) {
        url.fragment = std::string();
        state = url_state::fragment;
      } else {
        state = url_state::path;
        if (*it != '/') {
          --it;
        }
      }
    } else if (state == url_state::path) {
      if (((it == last) || (*it == '/')) ||
          (url.is_special() && (*it == '\\')) ||
          (!state_override && ((*it == '?') || (*it == '#')))) {
        if (url.is_special() && (*it == '\\')) {
          url.validation_error = true;
        } else if (is_double_dot_path_segment(buffer)) {
          shorten_path(url);
          if (!((*it == '/') || (url.is_special() && (*it == '\\')))) {
            url.path.emplace_back();
          }
        } else if (
            is_single_dot_path_segment(buffer) &&
            !((*it == '/') || (url.is_special() && (*it == '\\')))) {
          url.path.emplace_back();
        } else if (!is_single_dot_path_segment(buffer)) {
          if ((url.scheme.compare("file") == 0) && url.path.empty() && is_windows_drive_letter(buffer)) {
            if (!url.host || !url.host.value().empty()) {
              url.validation_error = true;
              url.host = std::string();
            }
            buffer[1] = ':';
          } else {
            url.path.push_back(buffer);
          }
        }

        buffer.clear();

        if ((url.scheme.compare("file") == 0) && ((it == last) || (*it == '?') || (*it == '#'))) {
          auto path = std::deque<std::string>(begin(url.path), end(url.path));
          while ((path.size() > 1) && path[0].empty()) {
            url.validation_error = true;
            path.pop_front();
          }
          url.path.assign(begin(path), end(path));
        }

        if (*it == '?') {
          url.query = std::string();
          state = url_state::query;
        }

        if (*it == '#') {
          url.fragment = std::string();
          state = url_state::fragment;
        }
      } else {
        detail::encode_char(*it, std::back_inserter(buffer), " \"<>`#?{}");
      }
    } else if (state == url_state::cannot_be_a_base_url_path) {
      if (*it == '?') {
        url.query = std::string();
        state = url_state::query;
      } else if (*it == '#') {
        url.fragment = std::string();
        state = url_state::fragment;
      } else {
        if ((it != last) && !is_url_code_point(*it) && (*it != '%')) {
          url.validation_error = true;
        }
        else if ((*it == '%') && !detail::is_pct_encoded(it, last)) {
          url.validation_error = true;
        }
        if (it != last) {
          auto el = std::string();
          detail::encode_char(*it, std::back_inserter(el));
          url.path[0] += el;
        }
      }
    } else if (state == url_state::query) {
      if (!state_override && (*it == '#')) {
        url.fragment = std::string();
        state = url_state::fragment;
      } else if (it != last) {
        detail::encode_char(*it, std::back_inserter(*url.query), " \"#<>`");
      }
    } else if (state == url_state::fragment) {
      if (*it == '\0') {
        url.validation_error = true;
      } else {
        detail::encode_char(*it, std::back_inserter(*url.fragment), " \"<>`");
      }
    }

    if (it == last) {
      break;
    }

    ++it;
  }

  url.success = true;
  return url;
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
}  // namespace skyr
