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

namespace skyr {
/// \brief
class ipv6_address {

  std::array<unsigned short, 8> repr;

 public:

  using repr_type = decltype(repr);
  using value_type = unsigned short;
  using iterator = unsigned short *;
  using const_iterator = const unsigned short *;
  using reference = unsigned short &;
  using const_reference = const unsigned short &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  /// \brief Constructor
  ipv6_address() {
    std::fill(std::begin(repr), std::end(repr), 0);
  }

  /// \brief Constructor
   explicit ipv6_address(std::array<unsigned short, 8> repr)
       : repr(repr) {}

   /// \returns
  std::string to_string() const;

};

/// \param input
/// \param returns
optional<ipv6_address> parse_ipv6_address(string_view input);
}  // namespace skyr

#endif //SKYR_IPV6_ADDRESS_INC
