// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV4_ADDRESS_INC
#define SKYR_IPV4_ADDRESS_INC

#include <string>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
/// Represents an IPv4 address.
class ipv4_address {

  unsigned int repr_;

 public:

  /// \brief Constructor
  ipv4_address()
      : repr_(0) {}

   /// \brief Constructor
   /// \param address
  explicit ipv4_address(unsigned int address)
      : repr_(address) {}

  /// \returns
  std::string to_string() const;

  ///

};

/// \exclude
namespace details {
enum class ipv4_address_errc {
  valid_domain,
  invalid,
};

expected<ipv4_address, ipv4_address_errc> parse_ipv4_address(string_view input);
}  // namespace details

/// \param input
/// \returns
optional<ipv4_address> parse_ipv4_address(string_view input);
}  // namespace skyr

#endif //SKYR_IPV4_ADDRESS_INC
