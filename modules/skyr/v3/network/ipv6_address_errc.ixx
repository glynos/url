// Copyright 2021 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

export module skyr.v3.network.ipv6_address_errc;

export {
  namespace skyr::inline v3 {
    /// Enumerates IPv6 address parsing errors
    enum class ipv6_address_errc {
      /// IPv6 address does not start with a double colon
      does_not_start_with_double_colon,
      /// IPv6 piece is not valid
      invalid_piece,
      /// IPv6 piece is not valid because address is expected to be
      /// compressed
      compress_expected,
      /// IPv4 segment is empty
      empty_ipv4_segment,
      /// IPv4 segment number is invalid
      invalid_ipv4_segment_number,
    };
  }  // namespace skyr::inline v3
}  // export
