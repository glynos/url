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
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>
#include <skyr/url/url_errors.hpp>

namespace skyr {
// https://url.spec.whatwg.org/#concept-basic-url-parser
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

struct url_record {
  std::string url;

  std::string scheme;
  std::string username;
  std::string password;
  optional<std::string> host;
  optional<std::uint16_t> port;
  std::vector<std::string> path;
  optional<std::string> query;
  optional<std::string> fragment;

  bool cannot_be_a_base_url;
  optional<std::string> object;

  bool success;
//  url_error error;
  bool validation_error;

  url_record()
      : url{}, cannot_be_a_base_url{false}, success{false}, validation_error{false} {}

  explicit operator bool () const noexcept {
    return success;
  }

  bool is_special() const;

  bool includes_credentials() const;

 private:

  void parse_scheme_start(char c);

};

url_record basic_parse(
    std::string input,
    const optional<url_record> &base = nullopt,
    const optional<url_record> &url = nullopt,
    optional<url_state> state_override = nullopt);

url_record parse(
    std::string input,
    const optional<url_record> &base = nullopt);

std::string serialize(
    const url_record &url,
    bool exclude_fragment = false);

/**
 * \class path_iterator
 */
class path_iterator {
 public:

  using value_type = string_view;
  using difference_type = std::ptrdiff_t;
  using pointer = const value_type *;
  using reference = const value_type &;
  using iterator_category = std::forward_iterator_tag;

  path_iterator() = default;

  explicit path_iterator(optional<string_view> path)
      : path_{path}, element_{} {
    if (path_ && path_.value().empty()) {
      path_ = nullopt;
    } else {
      assign();
    }
  }

  path_iterator(const path_iterator &) = default;

  path_iterator &operator=(const path_iterator &) = default;

  reference operator++() noexcept {
    increment();
    return element_;
  }

  value_type operator++(int) noexcept {
    auto original = element_;
    increment();
    return original;
  }

  reference operator*() const noexcept {
    return element_;
  }

  pointer operator->() const noexcept {
    return std::addressof(element_);
  }

  bool operator==(const path_iterator &other) const noexcept {
    if (!path_ && !other.path_) {
      return true;
    }
    else if (path_ && other.path_) {
      // since we're comparing substrings, the address of the first
      // element in each iterator must be the same
      return std::addressof(element_) == std::addressof(other.element_);
    }
    return false;
  }

  inline bool operator!=(const path_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void advance() noexcept {
    assert(path_ && "Invalid path");

    if (!path_.value().empty()) {
      auto first = std::begin(path_.value()), last = std::end(path_.value());

      auto sep_it =
          std::find_if(first, last, [](char c) -> bool {
            return (c == '/') || (c == '\\');
          });

      if (sep_it != last) {
        ++sep_it;  // skip next separator
      }

      // reassign path to the next element
      path_ = string_view(std::addressof(*sep_it), std::distance(sep_it, last));
    }
    else {
      path_ = nullopt;
    }
  }



  void assign() noexcept {
    if (path_) {
      auto first = std::begin(path_.value()), last = std::end(path_.value());

      auto sep_it =
          std::find_if(first, last, [](char c) -> bool {
            return (c == '/') || (c == '\\');
          });

      element_ =
          string_view(std::addressof(*first), std::distance(first, sep_it));
    }
  }

  void increment() noexcept {
    advance();
    assign();
  }

  optional<string_view> path_;
  value_type element_;
};
}  // namespace skyr

#endif  // NETWORK_DETAIL_URI_PARSE_INC
