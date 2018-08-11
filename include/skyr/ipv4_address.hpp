// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_IPV4_ADDRESS_INC
#define SKYR_IPV4_ADDRESS_INC

#include <string>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>
#include <skyr/optional.hpp>

namespace skyr {
/// This class represents an IPv4 address.
class ipv4_address {

  unsigned int address_;

 public:

  /// Constructor
  ipv4_address()
      : address_(0) {}

   /// Constructor
   /// \param address
  explicit ipv4_address(unsigned int address)
      : address_(address) {}

  /// \returns
  unsigned int address() const noexcept {
    return address_;
  }

  /// \returns The address as a string.
  std::string to_string() const;

};

///
enum class ipv4_address_errc {
  more_than_4_segments,
  empty_part,
  invalid_segment_number,
  validation_error,
};

/// \param input
/// \returns
expected<ipv4_address, ipv4_address_errc> parse_ipv4_address(string_view input);

/// \param input
/// \returns
expected<ipv4_address, ipv4_address_errc> parse_ipv4_address(std::string input);
}  // namespace skyr

#endif //SKYR_IPV4_ADDRESS_INC
