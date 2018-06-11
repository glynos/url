// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV6_ADDRESS_HPP
#define SKYR_IPV6_ADDRESS_HPP

#include <string>
#include <array>
#include <algorithm>
#include <iterator>
#include <skyr/string_view.hpp>

namespace skyr {
/**
 *
 */
class ipv6_address {

  std::array<unsigned short, 8> repr;

 public:

  using repr_type = decltype(repr);
  using value_type = unsigned short;
  using iterator = unsigned short *;
  using const_iterator = const unsigned short *;
  using reference = unsigned short &;
  using const_reference = const unsigned short &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  /**
   * @brief Constructor.
   */
  ipv6_address() {
    std::fill(std::begin(repr), std::end(repr), 0);
  }

  explicit ipv6_address(string_view address);

  /**
   *
   * @return
   */
  iterator begin() {
    return repr.begin();
  }

  /**
   *
   * @return
   */
  iterator end() {
    return repr.end();
  }

  /**
   *
   * @return
   */
  const_iterator begin() const {
    return repr.begin();
  }

  /**
   *
   * @return
   */
  const_iterator end() const {
    return repr.end();
  }

  /**
   *
   * @param index
   * @return
   */
  reference operator [] (size_type index) {
    return repr[index];
  }

  /**
   *
   * @param index
   * @return
   */
  value_type operator [] (size_type index) const {
    return repr[index];
  }

  /**
   *
   * @return
   */
  size_type size() const {
    return repr.size();
  }

  /**
   *
   * @return
   */
  std::string to_string() const;

};

optional<ipv6_address> parse_ipv6_address(string_view input);
}  // namespace skyr

#endif //SKYR_IPV6_ADDRESS_HPP
