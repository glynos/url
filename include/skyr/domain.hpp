// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_DOMAIN_HPP
#define SKYR_DOMAIN_HPP

#include <string>
#include <system_error>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
///
enum class domain_errc {
  disallowed_code_point = 1,
  bad_input,
  overflow,
  encoding_error,
  // invalid_length,
};
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::domain_errc> : true_type {};
}  // namespace std

namespace skyr {
///
/// \param error
/// \returns
std::error_code make_error_code(domain_errc error);

/// Punycode functions
namespace punycode {
/// \param input
/// \returns
expected<std::string, std::error_code> encode(string_view input);

/// \param input
/// \returns
expected<std::string, std::error_code> encode(u32string_view input);

/// \param input
/// \returns
expected<std::string, std::error_code> decode(string_view input);
}  // namespace punycode

/// \param domain
/// \param be_strict
/// returns
expected<std::string, std::error_code> domain_to_ascii(
    string_view domain,
    bool be_strict = true);

/// \param domain
/// \param be_strict
/// \returns
expected<std::string, std::error_code> domain_to_ascii(
    u32string_view domain,
    bool be_strict = true);
}  // namespace skyr

#endif //SKYR_DOMAIN_HPP
