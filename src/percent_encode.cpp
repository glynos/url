// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <cstring>
#include <cassert>
#include <algorithm>
#include "skyr/percent_encode.hpp"
#include "skyr/unicode.hpp"

namespace skyr {
namespace {
class ipv4_address_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *ipv4_address_error_category::name() const noexcept {
  return "percent encoding";
}

std::string ipv4_address_error_category::message(int error) const noexcept {
  switch (static_cast<percent_encode_errc>(error)) {
    case percent_encode_errc::non_hex_input:
      return "Non hex input";
    case percent_encode_errc::overflow:
      return "Overflow";
    default:
      return "(Unknown error)";
  }
}

static const ipv4_address_error_category category{};
}  // namespace

std::error_code make_error_code(percent_encode_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
inline char hex_to_letter(char byte) {
  if ((byte >= 0) && (byte < 10)) {
    return byte + '0';
  }

  if ((byte >= 10) && (byte < 16)) {
    return byte - char(10) + 'A';
  }

  return byte;
}

inline expected<char, std::error_code> letter_to_hex(char byte) {
  if ((byte >= '0') && (byte <= '9')) {
    return byte - '0';
  }

  if ((byte >= 'a') && (byte <= 'f')) {
    return byte + char(10) - 'a';
  }

  if ((byte >= 'A') && (byte <= 'F')) {
    return byte + char(10) - 'A';
  }

  return make_unexpected(make_error_code(
      percent_encode_errc::non_hex_input));
}
}  // namespace

std::string percent_encode_byte(char byte, const exclude_set &excludes) {
  auto encoded = std::string{};
  if (excludes.contains(byte)) {
    encoded += '%';
    encoded += hex_to_letter((byte >> 4) & 0x0f);
    encoded += hex_to_letter(byte & 0x0f);
  }
  else {
    encoded += static_cast<char>(byte);
  }
  return encoded;
}

expected<std::string, std::error_code> percent_encode(
    std::string_view input, const exclude_set &excludes) {
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
    std::u32string_view input, const exclude_set &excludes) {
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
  ++it;
  auto h0 = *it;
  auto v0 = letter_to_hex(h0);
  if (!v0) {
    return make_unexpected(std::move(v0.error()));
  }

  ++it;
  auto h1 = *it;
  auto v1 = letter_to_hex(h1);
  if (!v1) {
    return make_unexpected(std::move(v1.error()));
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
