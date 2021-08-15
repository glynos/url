// Copyright 2021 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

export module skyr.v3.network.ipv4_address_errc;

export {
  namespace skyr::inline v3 {
    /// Enumerates IPv4 address parsing errors
    enum class ipv4_address_errc {
      /// The input contains more than 4 segments
      too_many_segments,
      /// The input contains an empty segment
      empty_segment,
      /// The segment numers invalid
      invalid_segment_number,
      /// Overflow
      overflow,
    };
  }  // namespace skyr::inline v3
}  // export
