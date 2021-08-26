// Copyright 2020-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bit>
#include <type_traits>
#include <array>

namespace skyr::inline v2::details {
  template <class intT>
  requires std::is_integral_v<intT> constexpr inline auto swap_endianness(intT v) noexcept -> intT {
    constexpr auto byte_count = sizeof(v);
    constexpr auto bit_count = 8ul;
    std::array<std::byte, byte_count> bytes{};
    for (auto i = 0ul; i < byte_count; ++i) {
      bytes[i] = static_cast<std::byte>(v >> (i * bit_count));
    }
    return *static_cast<const intT *>(static_cast<const void *>(bytes.data()));
  }
}  // namespace skyr::inline v2::details

export module skyr.v3.platform.endianness;

export {
  namespace skyr::inline v2 {
    template <class intT>
      requires std::is_integral_v<intT> constexpr inline auto to_network_byte_order(intT v) noexcept -> intT {
      return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
    }

    template <class intT>
    requires std::is_integral_v<intT> constexpr inline auto from_network_byte_order(intT v) noexcept -> intT {
      return (std::endian::big == std::endian::native) ? v : details::swap_endianness(v);  // NOLINT
    }
  }  // namespace skyr::inline v2
} // export
