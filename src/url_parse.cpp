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
#include "skyr/url_state.hpp"
#include "skyr/url_parse.hpp"
#include "url_parser_context.hpp"

namespace skyr {
optional<url_record> basic_parse(
    std::string input,
    const optional<url_record> &base,
    const optional<url_record> &url,
    optional<url_state> state_override) {
//  if (input.empty()) {
//    return url_record();
//  }
//
  using url_parse_func = std::function<url_parse_action (url_parser_context &, char)>;
  using url_parse_funcs = std::map<url_state, url_parse_func>;

  auto parse_funcs = url_parse_funcs{
      {url_state::scheme_start,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_scheme_start(c);
       }},
      {url_state::scheme,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_scheme(c);
       }},
      {url_state::no_scheme,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_no_scheme(c);
       }},
      {url_state::special_relative_or_authority,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_special_relative_or_authority(c);
       }},
      {url_state::path_or_authority,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_path_or_authority(c);
       }},
      {url_state::relative,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_relative(c);
       }},
      {url_state::relative_slash,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_relative_slash(c);
       }},
      {url_state::special_authority_slashes,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_special_authority_slashes(c);
       }},
      {url_state::special_authority_ignore_slashes,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_special_authority_ignore_slashes(c);
       }},
      {url_state::authority,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_authority(c);
       }},
      {url_state::host,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_hostname(c);
       }},
      {url_state::port,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_port(c);
       }},
      {url_state::file,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_file(c);
       }},
      {url_state::file_slash,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_file_slash(c);
       }},
      {url_state::file_host,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_file_host(c);
       }},
      {url_state::path_start,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_path_start(c);
       }},
      {url_state::path,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_path(c);
       }},
      {url_state::cannot_be_a_base_url_path,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_cannot_be_a_base_url(c);
       }},
      {url_state::query,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_query(c);
       }},
      {url_state::fragment,
       [](url_parser_context &context, char c) -> url_parse_action {
         return context.parse_fragment(c);
       }}};

  auto context = url_parser_context(input, base, url, state_override);

  while (true) {
    auto func = parse_funcs[context.state];
    auto action = func(context, *context.it);
    switch (action) {
      case url_parse_action::increment:
        break;
      case url_parse_action::continue_:
        continue;
      case url_parse_action::fail:
        return nullopt;
      case url_parse_action::success:
        return context.url;
    }

    if (context.is_eof()) {
      break;
    }

    context.increment();
  }

  return context.url;
}

optional<url_record> parse(
    std::string input,
    const optional<url_record> &base) {
  auto url = basic_parse(input, base);

  if (!url) {
    return url;
  }

  if (url.value().scheme.compare("blob") != 0) {
    return url;
  }

  if (url.value().path.empty()) {
    return url;
  }

  // TODO: check Blob URL store

  return url;
}

std::string serialize(
    const url_record &url,
    bool exclude_fragment) {
  auto output = url.scheme + ":";

  if (url.host) {
    if (url.includes_credentials()) {
      output += url.username;
      if (!url.password.empty()) {
        output += ":";
        output += url.password;
      }
      output += "@";

      // TODO: serialize host
      output += url.host.value();

      if (url.port) {
        output += ":";
        output += std::to_string(url.port.value());
      }
    }
  }
  else if (!url.host && (url.scheme.compare("file") == 0)) {
    output += "//";
  }

  if (url.cannot_be_a_base_url) {
    output += url.path.front();
  }
  else {
    for (const auto &segment : url.path) {
      output += "/";
      output += segment;
    }
  }

  if (url.query) {
    output += "?";
    output += url.query.value();
  }

  if (!exclude_fragment && url.fragment) {
    output += "#";
    output += url.fragment.value();
  }

  return output;
}
}  // namespace skyr
