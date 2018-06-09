// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef URI_URL_CONTEXT_HPP
#define URI_URL_CONTEXT_HPP

#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>
#include <skyr/url_parse.hpp>
#include "skyr/url_record.hpp"

namespace skyr {
enum class url_parse_action {
  increment,
  continue_,
  fail,
  success,
};


class url_parser_context {

 private:

  std::string input;

  skyr::string_view view;
//  skyr::string_view::const_iterator first;
//  skyr::string_view::const_iterator last;

 public:

  skyr::string_view::const_iterator it;

  skyr::optional<url_record> base;
  url_record url;

  skyr::url_state state;
  skyr::optional<skyr::url_state> state_override;

  std::string buffer;

  bool at_flag;
  bool square_braces_flag;
  bool password_token_seen_flag;

  bool validation_error;

  url_parser_context(
      std::__1::string input,
      const skyr::optional<url_record> &base,
      const skyr::optional<url_record> &url,
      skyr::optional<skyr::url_state> state_override = skyr::nullopt);

  //url_parse_action parse_part(char c);

  bool is_eof() const {
    return it == end(view);
  }

  void increment() {
    ++it;
  }

  void decrement() {
    --it;
  }

  void reset() {
    it = begin(view);
  }

  void restart_from_buffer() {
    it = it - buffer.size() - 1;
  }

  url_parse_action parse_scheme_start(char c);
  url_parse_action parse_scheme(char c);
  url_parse_action parse_no_scheme(char c);
  url_parse_action parse_special_relative_or_authority(char c);
  url_parse_action parse_path_or_authority(char c);
  url_parse_action parse_relative(char c);
  url_parse_action parse_relative_slash(char c);
  url_parse_action parse_special_authority_slashes(char c);
  url_parse_action parse_special_authority_ignore_slashes(char c);
  url_parse_action parse_authority(char c);
  url_parse_action parse_hostname(char c);
  url_parse_action parse_port(char c);
  url_parse_action parse_file(char c);
  url_parse_action parse_file_slash(char c);
  url_parse_action parse_file_host(char c);
  url_parse_action parse_path_start(char c);
  url_parse_action parse_path(char c);
  url_parse_action parse_cannot_be_a_base_url(char c);
  url_parse_action parse_query(char c);
  url_parse_action parse_fragment(char c);

};
}  // namespace skyr

#endif //URI_URL_CONTEXT_HPP
