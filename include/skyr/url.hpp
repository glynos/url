// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_INC
#define SKYR_URL_INC

#include <string>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>
#include <skyr/url_record.hpp>
#include <skyr/url_error.hpp>

#ifdef NETWORK_URI_MSVC
#pragma warning(push)
#pragma warning(disable : 4251 4231 4660)
#endif

namespace skyr {
/// This class repesents a URL.
class url {
 public:

  using value_type = string_view::value_type;
  using iterator = string_view::iterator;
  using const_iterator = string_view::const_iterator;
  using reference = string_view::reference;
  using const_reference = string_view::const_reference;
  using size_type = string_view::size_type;
  using difference_type = string_view::difference_type;

  /// Default constructor
  url() = default;

  /// Constructor
  /// \param input The input string
  /// \throws `type_error`
  explicit url(std::string input);

  /// Constructor
  /// \param input The input string
  /// \param base A base URL
  /// \throws `type_error`
  url(std::string input, std::string base);

  /// Constructor
  /// \param input A URL record
  explicit url(url_record &&input) noexcept;

  /// \returns
  std::string href() const;

  /// \returns
  std::string origin() const;

  /// \returns
  std::string protocol() const;

  /// \returns
  std::string username() const;

  /// \returns
  std::string password() const;

  /// \returns
  std::string host() const;

  /// \returns
  std::string hostname() const;

  /// \returns
  std::string port() const;

  /// \returns
  template <typename intT>
  intT port(typename std::is_integral<intT>::type * = nullptr) const {
    auto p = port();
    const char *port_first = p.data();
    char *port_last = nullptr;
    return static_cast<intT>(std::strtoul(port_first, &port_last, 10));
  }

  /// \returns
  std::string pathname() const;

  /// \returns
  std::string search() const;

  /// \returns
  std::string hash() const;

  /// \returns
  bool is_special() const;

  /// \returns
  bool validation_error() const;

  /// \returns
  std::string serialize() const;

  /// \returns
  const_iterator begin() const noexcept {
    return view_.begin();
  }

  /// \returns
  const_iterator end() const noexcept {
    return view_.end();
  }

  /// \returns
  bool empty() const noexcept {
    return view_.empty();
  }

  /// \returns
  size_type size() const noexcept {
    return view_.size();
  }

  /// \returns
  size_type length() const noexcept {
    return view_.length();
  }

  /// \param other
  /// \returns
  int compare(const url &other) const noexcept {
    return view_.compare(other.view_);
  }

  /// \param scheme
  /// \returns
  static optional<std::uint16_t> default_port(const std::string &scheme) noexcept;

 private:
  url_record url_;
  string_view view_;

  // query object
};

/// \param input
/// \returns
expected<url, parse_error> make_url(std::string input) noexcept;

/// \param input
/// \param base
/// \returns
expected<url, parse_error> make_url(std::string input, std::string base) noexcept;

/// Equality operator
/// \param lhs
/// \param rhs
/// \returns
inline bool operator == (const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/// Inequality operator
/// \param lhs
/// \param rhs
/// \returns
inline bool operator != (const url &lhs, const url &rhs) noexcept {
  return !(lhs == rhs);
}
}  // namespace skyr

#endif  // SKYR_URL_INC
