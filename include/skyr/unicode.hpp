// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_HPP
#define SKYR_UNICODE_HPP

#include <string>
#include <string_view>
#include <system_error>
#include <skyr/expected.hpp>

namespace skyr {
enum class unicode_errc {
  overflow,
  illegal_byte_sequence,
};
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::unicode_errc> : true_type {};
}  // namespace std

namespace skyr {
/// \param
/// \returns
std::error_code make_error_code(unicode_errc error);

/// \param
/// \returns
expected<std::string, std::error_code> wstring_to_bytes(std::wstring_view input);

/// \param
/// \returns
expected<std::wstring, std::error_code> wstring_from_bytes(std::string_view input);

/// \param
/// \returns
expected<std::u16string, std::error_code> utf16_from_bytes(std::string_view input);

/// \param
/// \returns
expected<std::string, std::error_code> utf16_to_bytes(std::u16string_view input);

/// \param
/// \returns
expected<std::u32string, std::error_code> utf32_from_bytes(std::string_view input);

/// \param
/// \returns
expected<std::string, std::error_code> utf32_to_bytes(std::u32string_view input);
}  // namespace skyr

#endif //SKYR_UNICODE_HPP
