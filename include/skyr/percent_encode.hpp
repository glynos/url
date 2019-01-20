// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PERCENT_ENCODE_INC
#define SKYR_URL_PERCENT_ENCODE_INC

#include <string>
#include <string_view>
#include <locale>
#include <set>
#include <cstddef>
#include <skyr/expected.hpp>

namespace skyr {
/// Enumerates percent encoding errors
enum class percent_encode_errc {
  /// Input was not a hex value
  non_hex_input,
  /// Overflow
  overflow,
};

enum class encode_set {
  c0_control = 1,
  fragment,
  query,
  path,
  userinfo,
};

/// Creates a `std::error_code` given a `skyr::percent_encode_errc`
/// value
/// \param error A percent encoding error
/// \returns A `std::error_code` object
std::error_code make_error_code(percent_encode_errc error);

/// Percent encodes a byte
/// \param byte The input byte
/// \returns A percent encoded string
std::string percent_encode_byte(char byte);

/// Percent encodes a byte if it is not in the exclude set
/// \param byte The input byte
/// \param excludes The set of code points to exclude when percent
///        encoding
/// \returns A percent encoded string if `byte` is not in the
///          exclude set, `byte` as a string otherwise
std::string percent_encode_byte(
    char byte, encode_set excludes);

/// Percent encodes a string
/// \param input A string of bytes
/// \param excludes The set of code points to exclude when percent
///        encoding
/// \returns A percent encoded ASCII string, or an error on failure
expected<std::string, std::error_code> percent_encode(
    std::string_view input, encode_set excludes);

/// Percent encodes a string
/// \param input A UTF-32 string
/// \param excludes The set of code points to exclude when percent
///        encoding
/// \returns A percent encoded ASCII string, or an error on failure
expected<std::string, std::error_code> percent_encode(
    std::u32string_view input, encode_set excludes);

/// Percent decode an already encoded string into a byte value
/// \param input An string of the form %XX, where X is a hexadecimal
///        value
/// \returns The percent decoded byte, or an error on failure
expected<char, std::error_code> percent_decode_byte(
    std::string_view input);

/// Percent decodes a string
/// \param input An ASCII string
/// \returns A UTF-8 string, or an error on failure
expected<std::string, std::error_code> percent_decode(
    std::string_view input);

/// Tests whether the input string contains percent encoded values
/// \param input An ASCII string
/// \param locale A locale
/// \returns `true` if the input string contains percent encoded
///          values, `false` otherwise
bool is_percent_encoded(
    std::string_view input,
    const std::locale &locale = std::locale::classic());
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::percent_encode_errc> : true_type {};
}  // namespace std

#endif  // SKYR_URL_PERCENT_ENCODE_INC
