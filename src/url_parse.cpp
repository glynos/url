// Copyright 2016-2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <limits>
#include <cmath>
#include <sstream>
#include <deque>
#include <map>
#include "skyr/url_parse_state.hpp"
#include "skyr/url_parse.hpp"
#include "url_parser_context.hpp"
#include "skyr/url_serialize.hpp"
#include "skyr/url_error.hpp"

namespace skyr {
namespace details {
expected<url_record, url_parse_errc> basic_parse(
    std::string input,
    const optional<url_record> &base,
    const optional<url_record> &url,
    optional<url_parse_state> state_override) {
  using url_parse_func = std::function<url_parse_action (url_parser_context &, char)>;
  using url_parse_funcs = std::map<url_parse_state, url_parse_func>;

  auto parse_funcs = url_parse_funcs{
      {url_parse_state::scheme_start,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_scheme_start(c);
       }},
      {url_parse_state::scheme,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_scheme(c);
       }},
      {url_parse_state::no_scheme,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_no_scheme(c);
       }},
      {url_parse_state::special_relative_or_authority,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_special_relative_or_authority(c);
       }},
      {url_parse_state::path_or_authority,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_path_or_authority(c);
       }},
      {url_parse_state::relative,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_relative(c);
       }},
      {url_parse_state::relative_slash,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_relative_slash(c);
       }},
      {url_parse_state::special_authority_slashes,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_special_authority_slashes(c);
       }},
      {url_parse_state::special_authority_ignore_slashes,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_special_authority_ignore_slashes(c);
       }},
      {url_parse_state::authority,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_authority(c);
       }},
      {url_parse_state::host,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_hostname(c);
       }},
      {url_parse_state::hostname,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_hostname(c);
       }},
      {url_parse_state::port,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_port(c);
       }},
      {url_parse_state::file,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_file(c);
       }},
      {url_parse_state::file_slash,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_file_slash(c);
       }},
      {url_parse_state::file_host,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_file_host(c);
       }},
      {url_parse_state::path_start,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_path_start(c);
       }},
      {url_parse_state::path,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_path(c);
       }},
      {url_parse_state::cannot_be_a_base_url_path,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_cannot_be_a_base_url(c);
       }},
      {url_parse_state::query,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_query(c);
       }},
      {url_parse_state::fragment,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_fragment(c);
       }}};

  auto context = url_parser_context(input, base, url, state_override);

  while (true) {
    auto func = parse_funcs[context.state];
    auto action = func(context, *context.it);
    switch (action) {
      case url_parse_action::success:
        return context.url;
      case url_parse_action::increment:
        break;
      case url_parse_action::continue_:
        continue;
      case url_parse_action::invalid_scheme:
      case url_parse_action::invalid_hostname:
      case url_parse_action::cannot_be_a_base_url:
      case url_parse_action::cannot_have_a_username_password_or_port:
      case url_parse_action::invalid_port:
        return make_unexpected(url_parse_errc(static_cast<int>(action)));
    }

    if (context.is_eof()) {
      break;
    }

    context.increment();
  }

  return context.url;
}
}  // namespace details

expected<url_record, url_parse_errc> parse(
    std::string input,
    const optional<url_record> &base) {
  auto url = details::basic_parse(input, base);

  if (!url) {
    return url;
  }

  if (url.value().scheme.compare("blob") != 0) {
    return url;
  }

  if (url.value().path.empty()) {
    return url;
  }

  return url;
}
}  // namespace skyr
