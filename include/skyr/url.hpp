// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_INC
#define SKYR_URL_INC

#include <string>
#include <stdexcept>
#include <array>
#include "skyr/string_view.hpp"
#include "skyr/url_record.hpp"

#ifdef NETWORK_URI_MSVC
#pragma warning(push)
#pragma warning(disable : 4251 4231 4660)
#endif

namespace skyr {
/// This exception is used when there is an error parsing the URL.
class type_error : public std::runtime_error {
 public:
   /// Constructor
  type_error() : std::runtime_error("Type error") {}
};

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
  const_iterator begin() const {
    return view_.begin();
  }

  /// \returns
  const_iterator end() const {
    return view_.end();
  }

  /// \returns
  bool empty() const {
    return view_.empty();
  }

  /// \returns
  size_type size() const {
    return view_.size();
  }

  /// \returns
  size_type length() const {
    return view_.length();
  }

  /// \param scheme
  /// \returns
  static optional<std::uint16_t> default_port(const std::string &scheme);

 private:
  url_record url_;
  string_view view_;

  // query object
};
}  // namespace skyr

#endif  // SKYR_URL_INC
