// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_INC
#define SKYR_URL_INC

#include <string>
#include <string_view>
#include <skyr/config.hpp>
#include <skyr/expected.hpp>
#include <skyr/url_record.hpp>
#include <skyr/url_error.hpp>
#include <skyr/url_search_parameters.hpp>
#include <skyr/details/translate.hpp>

#ifdef SKYR_URI_MSVC
#pragma warning(push)
#pragma warning(disable : 4251 4231 4660)
#endif

/// \namespace skyr
/// Top-level namespace for URL parsing, unicode encoding and domain
/// name parsing.
namespace skyr {
/// Thrown when there is an error parsing the URL
class url_parse_error : public std::runtime_error {
 public:
  /// Constructor
  /// \param error An error code value
  explicit url_parse_error(std::error_code error) noexcept
      : runtime_error("URL parse error"), error_(std::move(error)) {}

  /// \returns An error code
  std::error_code error() const noexcept {
    return error_;
  }

 private:

  std::error_code error_;

};

/// Represents a URL. Parsing is performed according to the
/// [WhatWG specification](https://url.spec.whatwg.org/)
class url {
 public:

  /// The underyling ASCII string type, or `std::basic_string<value_type>`
  using string_type = std::string;
  /// The internal string_view, or `std::basic_string_view<value_type>`
  using string_view = std::string_view;
  /// The ASCII character type
  using value_type = string_view::value_type;
  /// A constant iterator with a value type of `value_type`
  using const_iterator = string_view::const_iterator;
  /// An alias to `const_iterator`
  using iterator = const_iterator;
  /// A constant reference with value type of `value_type`
  using const_reference = string_view::const_reference;
  /// An alias to `const_reference`
  using reference = const_reference;

  /// Constructs an empty `url` object
  ///
  /// \post `empty()`
  url();

  /// Parses a URL from the input string
  ///
  /// \param input The input string
  /// \throws url_parse_error on parse errors
  explicit url(string_type &&input) {
    initialize(std::move(input));
  }

  /// Parses a URL from the input string. The input string can be
  /// any unicode encoded string (UTF-8, UTF-16 or UTF-32).
  ///
  /// \tparam Source The input string type
  /// \param input The input string
  /// \throws unicode_error on unicode decoding errors
  /// \throws url_parse_error on parse errors
  template<class Source>
  explicit url(const Source &input) {
    initialize(details::translate(input));
  }

  /// Parses a URL from the input string. The input string can be
  /// any unicode encoded string (UTF-8, UTF-16 or UTF-32).
  ///
  /// \tparam Source The input string type
  /// \param input The input string
  /// \param base A base URL
  /// \throws unicode_error on unicode decoding errors
  /// \throws url_parse_error on parse errors
  template<class Source>
  url(const Source &input, const url &base) {
    initialize(details::translate(input), base.url_);
  }

  /// Constructs a URL from an existing record
  ///
  /// \param input A URL record
  explicit url(url_record &&input) noexcept;

  /// Swaps this `url` object with another
  ///
  /// \param other The other `url` object
  void swap(url &other) noexcept;

  /// The URL string
  ///
  /// \returns The underlying URL string
  string_type href() const;

  ///
  /// \tparam Source The input string type.
  /// \param input The input string.
  /// \returns An error if the input is invalid.
  template <class Source>
  expected<void, std::error_code> set_href(const Source &input) {
    return set_href(details::translate(input));
  }

  ///
  /// \returns
  string_type to_json() const;

  ///
  /// \returns
  string_type protocol() const;

  ///
  /// \param protocol
  template <class Source>
  expected<void, std::error_code> set_protocol(const Source &protocol) {
    return set_protocol(details::translate(protocol));
  }

  ///
  /// \returns
  string_type username() const;

  ///
  /// \param username
  template <class Source>
  expected<void, std::error_code> set_username(const Source &username) {
    return set_username(details::translate(username));
  }

  ///
  /// \returns
  string_type password() const;

  ///
  /// \param password
  template <class Source>
  expected<void, std::error_code> set_password(const Source &password) {
    return set_password(details::translate(password));
  }

  ///
  /// \returns
  string_type host() const;

  ///
  /// \param host
  template <class Source>
  expected<void, std::error_code> set_host(const Source &host) {
    return set_host(details::translate(host));
  }

  ///
  /// \returns
  string_type hostname() const;

  ///
  /// \param hostname
  template <class Source>
  expected<void, std::error_code> set_hostname(const Source &hostname) {
    return set_hostname(details::translate(hostname));
  }

  ///
  /// \returns
  string_type port() const;

  ///
  /// \returns
  template<typename intT>
  intT port(typename std::is_integral<intT>::type * = nullptr) const {
    auto p = port();
    const char *port_first = p.data();
    char *port_last = nullptr;
    return static_cast<intT>(
        std::strtoul(port_first, &port_last, 10));
  }

  ///
  /// \param port
  /// \returns
  template <class Source>
  expected<void, std::error_code> set_port(const Source &port) {
    return set_port(details::translate(port));
  }

  ///
  /// \returns
  string_type pathname() const;

  ///
  /// \param pathname
  /// \returns
  template <class Source>
  expected<void, std::error_code> set_pathname(const Source &pathname) {
    return set_pathname(details::translate(pathname));
  }

  ///
  /// \returns
  string_type search() const;

  ///
  /// \param search
  /// \returns
  template <class Source>
  expected<void, std::error_code> set_search(const Source &search) {
    return set_search(details::translate(search));
  }

  ///
  /// \returns
  url_search_parameters &search_parameters();

  ///
  /// \returns
  string_type hash() const;

  ///
  /// \param hash
  /// \returns
  template <class Source>
  expected<void, std::error_code> set_hash(const Source &hash) {
    return set_hash(details::translate(hash));
  }

  ///
  /// \returns
  url_record record() const;

  ///
  /// \returns
  bool is_special() const noexcept;

  ///
  /// \returns
  bool validation_error() const noexcept;

  ///
  /// \returns
  const_iterator begin() const noexcept {
    return view_.begin();
  }

  ///
  /// \returns
  const_iterator end() const noexcept {
    return view_.end();
  }

  ///
  /// \returns
  string_view view() const noexcept {
    return view_;
  }

  /// Tests whether the URL is an empty string
  ///
  /// \returns `true` if the URL is an empty string, `false`
  ///          otherwise
  bool empty() const noexcept {
    return view_.empty();
  }

  /// Compares this `url` object lexicographically with another
  ///
  /// \param other The other `url` object
  /// \returns `href_.compare(other.href_)`
  int compare(const url &other) const noexcept {
    return view_.compare(other.view_);
  }

  ///
  /// \param scheme
  /// \returns
  static optional<std::uint16_t> default_port(const string_type &scheme) noexcept;

  /// Clears the underlying URL string
  void clear();

  /// Returns the underyling byte buffer
  ///
  /// \returns `href_.c_str()`
  const char *c_str() const noexcept;

  /// Returns the underlying string
  ///
  /// \returns `href_`
  operator string_type() const;

  /// Returns the URL as a `std::string`
  ///
  /// \returns A URL string
  std::string string() const;

  /// Returns the URL as a `std::string`
  /// \returns A URL string
  std::string u8string() const;

  /// Returns the URL as a `std::wstring`
  /// \returns A URL wstring
  std::wstring wstring() const;

  /// Returns the URL as a `std::16string`
  /// \returns A URL string
  std::u16string u16string() const;

  /// Returns the URL as a `std::u32string`
  /// \returns A URL u32string
  std::u32string u32string() const;

 private:

  void initialize(
      string_type &&input,
      optional<url_record> base = nullopt);
  void update_record(url_record &&record);
  expected<void, std::error_code> set_href(string_type &&href);
  expected<void, std::error_code> set_protocol(string_type &&protocol);
  expected<void, std::error_code> set_username(string_type &&username);
  expected<void, std::error_code> set_password(string_type &&password);
  expected<void, std::error_code> set_host(string_type &&host);
  expected<void, std::error_code> set_hostname(string_type &&hostname);
  expected<void, std::error_code> set_port(string_type &&port);
  expected<void, std::error_code> set_pathname(string_type &&pathname);
  expected<void, std::error_code> set_search(string_type &&search);
  expected<void, std::error_code> set_hash(string_type &&hash);

  url_record url_;
  std::string href_;
  string_view view_;
  url_search_parameters parameters_;
};

/// Swaps two `url` objects
///
/// Equivalent to `lhs.swap(rhs)`
///
/// \param lhs The first `url` object
/// \param rhs The second `url` object
void swap(url &lhs, url &rhs) noexcept;

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
namespace details {
expected<url, std::error_code> make_url(
    url::string_type &&input, optional<url_record> base);
}  // namespace details
#endif  // !defined(DOXYGEN_SHOULD_SKIP_THIS)

/// Parses a URL string and constructs a `url` object
///
/// \param input The input string
/// \returns A `url` object on success, an error on failure
inline expected<url, std::error_code> make_url(
    url::string_type &&input) {
  return details::make_url(std::move(input), nullopt);
}

/// Parses a URL string and constructs a `url` object
///
/// \tparam Source The input string type
/// \param input The input string
/// \returns A `url` object on success, an error on failure
template<class Source>
inline expected<url, std::error_code> make_url(
    const Source &input) {
  return details::make_url(details::translate(input), nullopt);
}

/// Parses a URL string and constructs a `url` object
///
/// \param input The input string
/// \param base The base URL
/// \returns A `url` object on success, an error on failure
inline expected<url, std::error_code> make_url(
    url::string_type &&input, const url &base) {
  return details::make_url(std::move(input), base.record());
}

/// Parses a URL string and constructs a `url` object
////
/// \tparam Source The input string type
/// \param input The input string
/// \param base The base URL
/// \returns A `url` object on success, an error on failure
template<class Source>
inline expected<url, std::error_code> make_url(
    const Source &input, const url &base) {
  return details::make_url(details::translate(input), base.record());
}

/// Tests two URLs for equality according to the
/// [WhatWG specification](https://url.spec.whatwg.org/#url-equivalence)
///
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `true` if the `url` objects are equal, `false` otherwise
inline bool operator == (const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/// Tests two URLs for inequality
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns !(lhs == rhs
inline bool operator != (const url &lhs, const url &rhs) noexcept {
  return !(lhs == rhs);
}

/// Comparison operator
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `lhs.compare(rhs) < 0`
inline bool operator < (const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/// Comparison operator
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `lhs.compare(rhs) > 0`
inline bool operator > (const url &lhs, const url &rhs) noexcept {
  return rhs < lhs;
}

/// Comparison operator
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns `!(lhs > rhs)
inline bool operator <= (const url &lhs, const url &rhs) noexcept {
  return !(lhs > rhs);
}

/// Comparison operator
/// \param lhs A `url` object
/// \param rhs A `url` object
/// \returns !(lhs < rhs)
inline bool operator >= (const url &lhs, const url &rhs) noexcept {
  return !(lhs < rhs);
}
}  // namespace skyr

#endif  // SKYR_URL_INC
