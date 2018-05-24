// Copyright 2013-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_INC
#define NETWORK_DETAIL_URI_PARSE_INC

#include "network/uri/detail/uri_parts.hpp"

namespace network {
namespace detail {
struct uri_parts;

// https://url.spec.whatwg.org/#concept-basic-url-parser
enum class url_state {
  null,
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

struct url_result {
  std::string url;

  std::string scheme;
  std::string username;
  std::string password;
  std::string hostname;
  std::string port;
  std::string path;
  std::string query;
  std::string fragment;

  bool success;
  bool validation_error;
  // uri_parts parts;

  url_result()
      : url{}, success{false}, validation_error{false} {}

  explicit operator bool () const {
    return success;
  }
};

url_result parse(string_view::const_iterator &it, string_view::const_iterator last,
                 url_state state_override = url_state::null);
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_URI_PARSE_INC
