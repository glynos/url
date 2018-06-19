// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_WHATWG_URL_INC
#define NETWORK_URI_WHATWG_URL_INC

/**
 * \file
 * \brief Contains the url class.
 */

#include <string>
#include <stdexcept>
#include <array>
#include "skyr/url_record.hpp"

#ifdef NETWORK_URI_MSVC
#pragma warning(push)
#pragma warning(disable : 4251 4231 4660)
#endif

/**
 * @namespace skyr
 */
namespace skyr {
/**
 * @class type_error
 */
class type_error : public std::runtime_error {
 public:
  /**
   * @brief Constructor.
   */
  type_error() : std::runtime_error("Type error") {}
};

/**
 * @class url
 */
class url {
 public:

  using value_type = string_view::value_type;
  using iterator = string_view::iterator;
  using const_iterator = string_view::const_iterator;
  using reference = string_view::reference;
  using const_reference = string_view::const_reference;
  using size_type = string_view::size_type;
  using difference_type = string_view::difference_type;

  /**
   * @brief Constructor.
   */
  url() = default;

  /**
   * @brief Constructor.
   * @param input
   */
  explicit url(std::string input);

  /**
   * @brief Constructor.
   * @param input
   * @param base
   */
  url(std::string input, std::string base);

  /**
   *
   * @return
   */
  std::string href() const;

  /**
   *
   * @return
   */
  std::string origin() const;

  /**
   *
   * @return
   */
  std::string protocol() const;

  /**
   *
   * @return
   */
  std::string username() const;

  /**
   *
   * @return
   */
  std::string password() const;

  /**
   *
   * @return
   */
  std::string host() const;

  /**
   *
   * @return
   */
  std::string hostname() const;

  /**
   *
   * @return
   */
  std::string port() const;

  template <typename intT>
  intT port(typename std::is_integral<intT>::type * = nullptr) const {
    auto p = port();
    const char *port_first = p.data();
    char *port_last = nullptr;
    return static_cast<intT>(std::strtoul(port_first, &port_last, 10));
  }

  /**
   *
   * @return
   */
  std::string pathname() const;

  /**
   *
   * @return
   */
  std::string search() const;

  /**
   *
   * @return
   */
  std::string hash() const;

  /**
   *
   * @return
   */
  bool is_special() const;

  /**
   *
   * @return
   */
  bool validation_error() const;

  /**
   *
   * @return
   */
  const_iterator begin() const {
    return view_.begin();
  }

  /**
   *
   * @return
   */
  const_iterator end() const {
    return view_.end();
  }

  /**
   *
   * @return
   */
  bool empty() const {
    return view_.empty();
  }

  /**
   *
   * @return
   */
  size_type size() const {
    return view_.size();
  }

  /**
   *
   * @return
   */
  size_type length() const {
    return view_.length();
  }

  /**
   *
   * @param scheme
   * @return
   */
  static optional<std::uint16_t> default_port(const std::string &scheme);

 private:
  url_record url_;
  string_view view_;

  // query object
};
}  // namespace skyr

#endif  // NETWORK_URI_WHATWG_URL_INC
