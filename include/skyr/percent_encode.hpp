// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_ENCODE_INC
#define SKYR_URL_DETAILS_ENCODE_INC

#include <string>
#include <string_view>
#include <locale>
#include <skyr/expected.hpp>

namespace skyr {
///
enum class percent_encode_errc {
  non_hex_input,
  overflow,
};
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::percent_encode_errc> : true_type {};
}  // namespace std

namespace skyr {
///
/// \param error
/// \returns
std::error_code make_error_code(percent_encode_errc error);

/// \param in
/// \param includes
/// \returns
std::string percent_encode_byte(char in, const char *includes = "");

/// \param input
/// \returns
expected<std::string, std::error_code> percent_encode(std::string_view input, const char *includes = "");

/// \param input
/// \returns
expected<std::string, std::error_code> percent_encode(std::u32string_view input, const char *includes = "");

/// \param input
/// \returns
expected<char, std::error_code> percent_decode_byte(std::string_view input);

/// \param input
/// \returns
expected<std::string, std::error_code> percent_decode(std::string_view input);

/// \param input
/// \param locale
/// \returns
bool is_percent_encoded(
    std::string_view input,
    const std::locale &locale = std::locale::classic());
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_ENCODE_INC
