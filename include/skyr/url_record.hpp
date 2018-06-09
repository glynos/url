// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef URI_URL_RECORD_HPP
#define URI_URL_RECORD_HPP

#include <vector>
#include <iterator>
#include <string>
#include <cstdint>
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
///**
// *
// */
//class ipv4_address {
//
//  std::uint32_t repr;
//
// public:
//
//  ipv4_address()
//    : repr(0) {}
//
//  explicit ipv4_address(std::uint32_t address)
//    : repr(address) {}
//
//};

/**
 *
 */
class ipv6_address {

  std::array<std::uint16_t, 8> repr;

 public:

  using repr_type = decltype(repr);
  using value_type = repr_type::value_type;
  using iterator = repr_type::iterator;
  using const_iterator = repr_type::const_iterator;
  using reference = repr_type::reference;
  using const_reference = repr_type::const_reference;
  using size_type = repr_type::size_type;
  using difference_type = repr_type::difference_type;

  /**
   * @brief Constructor.
   */
  ipv6_address() {
    std::fill(std::begin(repr), std::end(repr), 0);
  }

  iterator begin() {
    return repr.begin();
  }

  iterator end() {
    return repr.end();
  }

  const_iterator begin() const {
    return repr.begin();
  }

  const_iterator end() const {
    return repr.end();
  }

  reference operator [] (size_type i) {
    return repr[i];
  }

  value_type operator [] (size_type i) const {
    return repr[i];
  }

  size_type size() const {
    return repr.size();
  }

};



/**
 * @class url_record
 */
struct url_record {
  std::string url;

  std::string scheme;
  std::string username;
  std::string password;
  skyr::optional<std::string> host;
  skyr::optional<std::uint16_t> port;
  std::vector<std::string> path;
  skyr::optional<std::string> query;
  skyr::optional<std::string> fragment;

  bool cannot_be_a_base_url;
  skyr::optional<std::string> object;

  /**
   * @brief Constructor.
   */
  url_record()
      : url{}, cannot_be_a_base_url{false} {}

  /**
   *
   * @return
   */
  bool is_special() const;

  /**
   *
   * @return
   */
  bool includes_credentials() const;

};
}  // namespace skyr

#endif //URI_URL_RECORD_HPP
