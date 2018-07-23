// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PARSE_STATE_INC
#define SKYR_URL_PARSE_STATE_INC

namespace skyr {
/// States of the URL parser
enum class url_parse_state {
  /// The scheme start
  scheme_start,
  /// The scheme
  scheme,
  /// For URLs that have no scheme (this may still be valid)
  no_scheme,
  ///
  special_relative_or_authority,
  ///
  path_or_authority,
  ///
  relative,
  ///
  relative_slash,
  ///
  special_authority_slashes,
  ///
  special_authority_ignore_slashes,
  ///
  authority,
  ///
  host,
  ///
  hostname,
  ///
  port,
  ///
  file,
  ///
  file_slash,
  ///
  file_host,
  ///
  path_start,
  ///
  path,
  ///
  cannot_be_a_base_url_path,
  ///
  query,
  ///
  fragment,
};
}  // namespace skyr

#endif // SKYR_URL_STATE_INC
