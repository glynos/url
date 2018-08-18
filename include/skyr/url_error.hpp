// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_ERROR_INC
#define SKYR_URL_ERROR_INC

#include <string>
#include <system_error>
#include <skyr/url_parse_state.hpp>

namespace skyr {
///
enum class url_parse_errc {
  invalid_scheme=1,
  not_an_absolute_url_with_fragment,
  empty_hostname,
  invalid_ipv4_address,
  invalid_ipv6_address,
  forbidden_host_point,
  cannot_decode_host_point,
  domain_error,
  cannot_be_a_base_url,
  cannot_have_a_username_password_or_port,
  invalid_port,
};
}  // namespace skyr

namespace std {
template <>
struct is_error_code_enum<skyr::url_parse_errc> : true_type {};
}  // namespace std

namespace skyr {
std::error_code make_error_code(url_parse_errc error);
}  // namespace skyr

#endif // SKYR_URL_ERROR_INC
