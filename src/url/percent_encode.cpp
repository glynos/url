// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <cstring>
#include <cassert>
#include <algorithm>
#include "skyr/url/percent_encode.hpp"
#include "skyr/unicode/unicode.hpp"

namespace skyr {
namespace {
class percent_encode_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *percent_encode_error_category::name() const noexcept {
  return "percent encoding";
}

std::string percent_encode_error_category::message(int error) const noexcept {
  switch (static_cast<percent_encode_errc>(error)) {
    case percent_encode_errc::non_hex_input:
      return "Non hex input";
    case percent_encode_errc::overflow:
      return "Overflow";
    default:
      return "(Unknown error)";
  }
}

static const percent_encode_error_category category{};
}  // namespace

std::error_code make_error_code(percent_encode_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
inline char hex_to_letter(char byte) {
  if ((byte >= 0x00) && (byte < 0x0a)) {
    return byte + '0';
  }

  if ((byte >= 0x0a) && (byte < 0x10)) {
    return byte - static_cast<char>(0x0a) + 'A';
  }

  return byte;
}

inline expected<char, std::error_code> letter_to_hex(char byte) {
  if ((byte >= '0') && (byte <= '9')) {
    return byte - '0';
  }

  if ((byte >= 'a') && (byte <= 'f')) {
    return byte + static_cast<char>(0x0a) - 'a';
  }

  if ((byte >= 'A') && (byte <= 'F')) {
    return byte + static_cast<char>(0x0a) - 'A';
  }

  return make_unexpected(make_error_code(
      percent_encode_errc::non_hex_input));
}

std::string percent_encode_byte(char byte) {
  auto encoded = std::string{};
  encoded += '%';
  encoded += hex_to_letter((byte >> 4) & 0x0f);
  encoded += hex_to_letter(byte & 0x0f);
  return encoded;
}

inline bool is_c0_control_byte(char byte) {
  return (byte <= 0x1f) || (byte > 0x7e);
}

inline bool is_fragment_byte(char byte) {
  static const auto set = std::set<char>{0x20, 0x22, 0x3c, 0x3e, 0x60};
  return is_c0_control_byte(byte) || (set.find(byte) != set.end());
}

inline bool is_query_byte(char byte) {
  return is_fragment_byte(byte) || (byte == 0x27);
}

inline bool is_path_byte(char byte) {
  static const auto set = std::set<char>{0x23, 0x3f, 0x7b, 0x7d};
  return is_fragment_byte(byte) || (set.find(byte) != set.end());
}

inline bool is_userinfo_byte(char byte) {
  static const auto set = std::set<char>{
    0x2f, 0x3a, 0x3b, 0x3d, 0x40, 0x5b, 0x5c, 0x5d, 0x5e, 0x7c};
  return is_path_byte(byte) || (set.find(byte) != set.end());
}

template <class Pred>
std::string percent_encode_byte(char byte, Pred pred) {
  auto result = std::string{};
  if (pred(byte)) {
    result = percent_encode_byte(byte);
  }
  else {
    result = {byte};
  }
  return result;
}
}  // namespace

std::string percent_encode_byte(char byte, encode_set excludes) {
  switch (excludes) {
    case encode_set::c0_control:
      return percent_encode_byte(byte, is_c0_control_byte);
    case encode_set::userinfo:
      return percent_encode_byte(byte, is_userinfo_byte);
    case encode_set::path:
      return percent_encode_byte(byte, is_path_byte);
    case encode_set::query:
      return percent_encode_byte(byte, is_query_byte);
    case encode_set::fragment:
      return percent_encode_byte(byte, is_fragment_byte);
  }
  return {};
}

expected<std::string, std::error_code> percent_encode(
    std::string_view input, encode_set excludes) {
  auto result = std::string{};
  auto first = begin(input), last = end(input);
  auto it = first;
  while (it != last) {
    result += percent_encode_byte(*it, excludes);
    ++it;
  }
  return result;
}

expected<std::string, std::error_code> percent_encode(
    std::u32string_view input, encode_set excludes) {
  auto bytes = unicode::utf32_to_bytes(input);
  if (!bytes) {
    return make_unexpected(make_error_code(
        percent_encode_errc::overflow));
  }
  return percent_encode(bytes.value(), excludes);
}

expected<char, std::error_code> percent_decode_byte(std::string_view input) {
  if ((input.size() < 3) || (input.front() != '%')) {
    return make_unexpected(make_error_code(
        percent_encode_errc::non_hex_input));
  }

  auto it = begin(input);
  auto v0 = letter_to_hex(*++it);
  auto v1 = letter_to_hex(*++it);

  if (!v0 || !v1) {
    return make_unexpected(make_error_code(
        percent_encode_errc::non_hex_input));
  }

  return (0x10 * v0.value()) + v1.value();
}

expected<std::string, std::error_code> percent_decode(std::string_view input) {
  auto result = std::string{};
  auto first = begin(input), last = end(input);
  auto it = first;
  while (it != last) {
    if (*it == '%') {
      if (std::distance(it, last) < 3) {
        result.push_back(*it);
        return result;
      }
      auto byte = percent_decode_byte(std::string_view(std::addressof(*it), 3));
      if (!byte) {
        return make_unexpected(std::move(byte.error()));
      }
      result.push_back(byte.value());
      it += 3;
    } else {
      result.push_back(*it++);
    }
  }
  return result;
}

bool is_percent_encoded(
    std::string_view input,
    const std::locale &locale) {
  auto first = begin(input), last = end(input);
  auto it = first;

  if (it == last) {
    return false;
  }

  if (*it == '%') {
    ++it;
    if (it != last) {
      if (std::isxdigit(*it, locale)) {
        ++it;
        if (it != last) {
          if (std::isxdigit(*it, locale)) {
            return true;
          }
        }
      }
    }
  }

  return false;
}
}  // namespace skyr
