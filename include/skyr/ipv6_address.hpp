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
#include <skyr/optional.hpp>

namespace skyr {
///
class ipv6_address {

  std::array<unsigned short, 8> repr_;

  using repr_type = decltype(repr_);

 public:

  /// Constructor
  ipv6_address()
      : repr_() {
    std::fill(std::begin(repr_), std::end(repr_), 0);
  }

  /// Constructor
  /// \param repr
  explicit ipv6_address(std::array<unsigned short, 8> repr)
      : repr_(repr) {}

   /// \returns
  std::string to_string() const;

};

/// \param input
/// \param returns
optional<ipv6_address> parse_ipv6_address(string_view input);
}  // namespace skyr

#endif //SKYR_IPV6_ADDRESS_INC
