// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_CONTEXT_HPP
#define SKYR_URL_CONTEXT_HPP

#include <cassert>
#include <string_view>
#include <skyr/optional.hpp>
#include <skyr/expected.hpp>
#include "skyr/url/url_error.hpp"
#include <skyr/url/url_record.hpp>
#include "url_parse_impl.hpp"

namespace skyr {
enum class url_parse_action {
  success = 0,
  increment,
  continue_,
};

class url_parser_context {

 private:

  std::string input;
  std::string_view view;

 public:

  std::string_view::const_iterator it;

  optional<url_record> base;
  url_record url;

  url_parse_state state;
  optional<url_parse_state> state_override;

  std::string buffer;

  bool at_flag;
  bool square_braces_flag;
  bool password_token_seen_flag;

  url_parser_context(
      std::string input,
      const optional<url_record> &base,
      const optional<url_record> &url,
      optional<url_parse_state> state_override = skyr::nullopt);

  bool is_eof() const noexcept {
    return it == end(view);
  }

  void increment() noexcept {
    assert(it != end(view));
    ++it;
  }

  void decrement() noexcept {
    assert(it != begin(view));
    --it;
  }

  void reset() noexcept {
    it = begin(view);
  }

  void restart_from_buffer() noexcept {
    it = it - buffer.size() - 1;
  }

  expected<url_parse_action, url_parse_errc> parse_scheme_start(char byte);
  expected<url_parse_action, url_parse_errc> parse_scheme(char byte);
  expected<url_parse_action, url_parse_errc> parse_no_scheme(char byte);
  expected<url_parse_action, url_parse_errc> parse_special_relative_or_authority(char byte);
  expected<url_parse_action, url_parse_errc> parse_path_or_authority(char byte);
  expected<url_parse_action, url_parse_errc> parse_relative(char byte);
  expected<url_parse_action, url_parse_errc> parse_relative_slash(char byte);
  expected<url_parse_action, url_parse_errc> parse_special_authority_slashes(char byte);
  expected<url_parse_action, url_parse_errc> parse_special_authority_ignore_slashes(char byte);
  expected<url_parse_action, url_parse_errc> parse_authority(char byte);
  expected<url_parse_action, url_parse_errc> parse_hostname(char byte);
  expected<url_parse_action, url_parse_errc> parse_port(char byte);
  expected<url_parse_action, url_parse_errc> parse_file(char byte);
  expected<url_parse_action, url_parse_errc> parse_file_slash(char byte);
  expected<url_parse_action, url_parse_errc> parse_file_host(char byte);
  expected<url_parse_action, url_parse_errc> parse_path_start(char byte);
  expected<url_parse_action, url_parse_errc> parse_path(char byte);
  expected<url_parse_action, url_parse_errc> parse_cannot_be_a_base_url(char byte);
  expected<url_parse_action, url_parse_errc> parse_query(char byte);
  expected<url_parse_action, url_parse_errc> parse_fragment(char byte);

};
}  // namespace skyr

#endif // SKYR_URL_CONTEXT_HPP
