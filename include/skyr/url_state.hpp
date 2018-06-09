// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef URI_URL_STATE_HPP
#define URI_URL_STATE_HPP

namespace skyr {
// https://url.spec.whatwg.org/#concept-basic-url-parser
/**
 *
 */
enum class url_state {
  scheme_start,
  scheme,
  no_scheme,
  special_relative_or_authority,
  path_or_authority,
  relative,
  relative_slash,
  special_authority_slashes,
  special_authority_ignore_slashes,
  authority,
  host,
  hostname,
  port,
  file,
  file_slash,
  file_host,
  path_start,
  path,
  cannot_be_a_base_url_path,
  query,
  fragment,
};
}  // namespace skyr

#endif //URI_URL_STATE_HPP
