// Copyright 2018-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <array>
#include <string>
#include <string_view>
#include <optional>
#include <cmath>
#include <locale>
#include <format>
#include <tl/expected.hpp>

import skyr.v3.platform.endianness;
import skyr.v3.containers.static_vector;
import skyr.v3.network.ipv4_address_errc;

namespace skyr::inline v3::details {
  constexpr inline auto parse_ipv4_number(std::string_view input, bool *validation_error)
      -> tl::expected<std::uint64_t, ipv4_address_errc> {
    auto base = 10;

    if ((input.size() >= 2) && (input[0] == '0') && (std::tolower(input[1], std::locale::classic()) == 'x')) {
      *validation_error |= true;
      input = input.substr(2);
      base = 16;
    } else if ((input.size() >= 2) && (input[0] == '0')) {
      *validation_error |= true;
      input = input.substr(1);
      base = 8;
    }

    if (input.empty()) {
      return 0ULL;
    }

    char *pos = const_cast<char *>(input.data()) + input.size();  // NOLINT
    auto number = std::strtoull(input.data(), &pos, base);
    if ((number == ULLONG_MAX) || (pos != input.data() + input.size())) {
      return tl::make_unexpected(ipv4_address_errc::invalid_segment_number);
    }
    return number;
  }
}  // namespace skyr::inline v3::details

export module skyr.v3.network.ipv4;

export {
  namespace skyr::inline v3 {
    /// Represents an IPv4 address
    class ipv4_address {
      unsigned int address_ = 0;

    public:
      /// Constructor
      constexpr ipv4_address() = default;

      /// Constructor
      /// \param address Sets the IPv4 address to `address`
      constexpr explicit ipv4_address(unsigned int address) : address_(to_network_byte_order(address)) {
      }

      /// The address value
      /// \returns The address value
      [[nodiscard]] constexpr auto address() const noexcept {
        return from_network_byte_order(address_);
      }

      /// The address in bytes in network byte order
      /// \returns The address in bytes
      [[nodiscard]] constexpr auto to_bytes() const noexcept -> std::array<unsigned char, 4> {
        return {{static_cast<unsigned char>(address_ >> 24u), static_cast<unsigned char>(address_ >> 16u),
                static_cast<unsigned char>(address_ >> 8u), static_cast<unsigned char>(address_)}};
      }

      /// \returns The address as a string
      [[nodiscard]] auto serialize() const -> std::string {
        using namespace std::string_literals;
        using namespace std::string_view_literals;

        constexpr auto separator = [](auto i) { return (i < 4) ? "."sv : ""sv; };

        auto output = ""s;
        auto n = address_;
        for (auto i = 1U; i <= 4U; ++i) {
          output = std::format("{}{}{}", separator(i), n % 256, output);
          n >>= 8;
        }
        return output;
      }
    };

    /// Parses an IPv4 address
    /// \param input An input string
    /// \returns An `ipv4_address` object or an error
    constexpr inline auto parse_ipv4_address(std::string_view input, bool *validation_error)
        -> tl::expected<ipv4_address, ipv4_address_errc> {
      using namespace std::string_view_literals;

      auto parts = static_vector<std::string_view, 8>{};
      auto in_first = std::cbegin(input), in_last = std::cend(input);
      auto in_it = in_first, in_it_last = in_first;
      while (in_it != in_last) {
        if (parts.size() == parts.max_size()) {
          *validation_error |= true;
          return tl::make_unexpected(ipv4_address_errc::too_many_segments);
        }

        if (*in_it == '.') {
          parts.push_back(std::string_view(std::addressof(*in_it_last), std::distance(in_it_last, in_it)));
          ++in_it;
          in_it_last = in_it;
        } else {
          ++in_it;
        }
      }
      parts.push_back(std::string_view(std::addressof(*in_it_last), std::distance(in_it_last, in_it)));

      if (parts.back().empty()) {
        *validation_error |= true;
        if (parts.size() > 1) {
          parts.pop_back();
        }
      }

      if (parts.size() > 4) {
        *validation_error |= true;
        return tl::make_unexpected(ipv4_address_errc::too_many_segments);
      }

      auto numbers = static_vector<std::uint64_t, 4>{};

      for (const auto &part : parts) {
        if (part.empty()) {
          *validation_error |= true;
          return tl::make_unexpected(ipv4_address_errc::empty_segment);
        }

        auto number = details::parse_ipv4_number(std::string_view(part), validation_error);
        if (!number) {
          *validation_error |= true;
          return tl::make_unexpected(ipv4_address_errc::invalid_segment_number);
        }

        numbers.push_back(number.value());
      }

      constexpr auto greater_than_255 = [](auto number) { return number > 255; };

      auto first = numbers.begin(), last = numbers.end();
      if (last != std::find_if(first, last, greater_than_255)) {
        *validation_error |= true;
      }

      auto numbers_last_but_one = numbers.end();
      --numbers_last_but_one;
      if (numbers_last_but_one != std::find_if(first, numbers_last_but_one, greater_than_255)) {
        *validation_error |= true;
        return tl::make_unexpected(ipv4_address_errc::overflow);
      }

      if (numbers.back() >= static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
        *validation_error |= true;
        return tl::make_unexpected(ipv4_address_errc::overflow);
      }

      auto ipv4 = numbers.back();
      numbers.pop_back();

      auto counter = 0UL;
      for (auto &&number : numbers) {
        ipv4 += number * static_cast<std::uint64_t>(std::pow(256, 3 - counter));
        ++counter;
      }
      return ipv4_address(static_cast<unsigned int>(ipv4));
    }
  }  // namespace skyr::inline v3
}  // export

