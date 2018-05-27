// Copyright 2013-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_INC
#define NETWORK_DETAIL_URI_PARSE_INC

#include <vector>
#include <string>
#include <utility>
#include <iterator>
#include <network/optional.hpp>
#include <network/string_view.hpp>

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

struct blob {

};

struct media_source {

};

struct url_record {
  std::string url;

  std::string scheme;
  std::string username;
  std::string password;
  optional<std::string> host;
  optional<std::uint16_t > port;
  std::vector<std::string> path;
  optional<std::string> query;
  optional<std::string> fragment;

  bool cannot_be_a_base_url;
  optional<std::string> object;

  bool success;
  bool validation_error;

  url_record()
      : url{}, cannot_be_a_base_url{false}, success{false}, validation_error{false} {}

  explicit operator bool () const {
    return success;
  }
};

url_record basic_parse(
    std::string input,
    const optional<url_record> &base = nullopt,
    const optional<url_record> &url = nullopt,
    url_state state_override = url_state::null);

url_record parse(
    std::string input,
    const optional<url_record> &base = nullopt);
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_URI_PARSE_INC
