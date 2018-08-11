// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV6_ADDRESS_INC
#define SKYR_IPV6_ADDRESS_INC

#include <string>
#include <array>
#include <algorithm>
#include <iterator>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
/// This class represents in IPv6 address.
class ipv6_address {

  std::array<unsigned short, 8> address_;

  using repr_type = decltype(address_);

 public:

  /// Constructor
  ipv6_address()
      : address_{{0, 0, 0, 0, 0, 0, 0, 0}} {}

  /// Constructor
  /// \param address
  explicit ipv6_address(std::array<unsigned short, 8> address)
      : address_(address) {}

   /// \returns The IPv4 address as a string.
  std::string to_string() const;

};

enum class ipv6_address_errc {
//  does_not_start_with_double_colon
//  invalid_index,
//  segment_length_is_zero,
  invalid,
};

/// \param input
/// \returns
expected<ipv6_address, ipv6_address_errc > parse_ipv6_address(string_view input);

/// \param input
/// \returns
expected<ipv6_address, ipv6_address_errc> parse_ipv6_address(std::string input);
}  // namespace skyr

#endif //SKYR_IPV6_ADDRESS_INC
