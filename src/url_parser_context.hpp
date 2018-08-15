// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_CONTEXT_HPP
#define SKYR_URL_CONTEXT_HPP

#include <skyr/optional.hpp>
#include <skyr/expected.hpp>
#include "skyr/url_error.hpp"
#include <skyr/string_view.hpp>
#include <skyr/url_parse_state.hpp>
#include <skyr/url_record.hpp>

namespace skyr {
enum class url_parse_action {
  success = 0,
  increment,
  continue_,
};

class url_parser_context {

 private:

  std::string input;
  skyr::string_view view;

 public:

  skyr::string_view::const_iterator it;

  skyr::optional<url_record> base;
  url_record url;

  skyr::url_parse_state state;
  skyr::optional<skyr::url_parse_state> state_override;

  std::string buffer;

  bool at_flag;
  bool square_braces_flag;
  bool password_token_seen_flag;

  bool validation_error;

  url_parser_context(
      std::string input,
      const skyr::optional<url_record> &base,
      const skyr::optional<url_record> &url,
      skyr::optional<skyr::url_parse_state> state_override = skyr::nullopt);

  bool is_eof() const noexcept {
    return it == end(view);
  }

  void increment() noexcept {
    ++it;
  }

  void decrement() noexcept {
    --it;
  }

  void reset() noexcept {
    it = begin(view);
  }

  void restart_from_buffer() noexcept {
    it = it - buffer.size() - 1;
  }

  expected<url_parse_action, url_parse_errc> parse_scheme_start(char c);
  expected<url_parse_action, url_parse_errc> parse_scheme(char c);
  expected<url_parse_action, url_parse_errc> parse_no_scheme(char c);
  expected<url_parse_action, url_parse_errc> parse_special_relative_or_authority(char c);
  expected<url_parse_action, url_parse_errc> parse_path_or_authority(char c);
  expected<url_parse_action, url_parse_errc> parse_relative(char c);
  expected<url_parse_action, url_parse_errc> parse_relative_slash(char c);
  expected<url_parse_action, url_parse_errc> parse_special_authority_slashes(char c);
  expected<url_parse_action, url_parse_errc> parse_special_authority_ignore_slashes(char c);
  expected<url_parse_action, url_parse_errc> parse_authority(char c);
  expected<url_parse_action, url_parse_errc> parse_hostname(char c);
  expected<url_parse_action, url_parse_errc> parse_port(char c);
  expected<url_parse_action, url_parse_errc> parse_file(char c);
  expected<url_parse_action, url_parse_errc> parse_file_slash(char c);
  expected<url_parse_action, url_parse_errc> parse_file_host(char c);
  expected<url_parse_action, url_parse_errc> parse_path_start(char c);
  expected<url_parse_action, url_parse_errc> parse_path(char c);
  expected<url_parse_action, url_parse_errc> parse_cannot_be_a_base_url(char c);
  expected<url_parse_action, url_parse_errc> parse_query(char c);
  expected<url_parse_action, url_parse_errc> parse_fragment(char c);

};
}  // namespace skyr

#endif // SKYR_URL_CONTEXT_HPP
