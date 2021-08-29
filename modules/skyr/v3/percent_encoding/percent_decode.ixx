// Copyright 2020-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <cstddef>
#include <cassert>
#include <string_view>
#include <tl/expected.hpp>

export module skyr.v3.percent_encoding.percent_decode;
export import skyr.v3.percent_encoding.percent_encode_errc;
export import skyr.v3.percent_encoding.sentinel;

namespace skyr::inline v3::percent_encoding::details {
  constexpr inline auto alnum_to_hex(char value) noexcept -> tl::expected<std::byte, percent_encode_errc> {
    if ((value >= '0') && (value <= '9')) {
      return static_cast<std::byte>(value - '0');
    }

    if ((value >= 'a') && (value <= 'f')) {
      return static_cast<std::byte>(value + '\x0a' - 'a');
    }

    if ((value >= 'A') && (value <= 'F')) {
      return static_cast<std::byte>(value + '\x0a' - 'A');
    }

    return tl::make_unexpected(percent_encode_errc::non_hex_input);
  }
}  // namespace skyr::inline v3::percent_encoding::details

export {
  namespace skyr::inline v3::percent_encoding {
    ///
    class percent_decode_iterator {
    public:
      ///
      using iterator_category = std::forward_iterator_tag;
      ///
      using value_type = tl::expected<char, percent_encode_errc>;
      ///
      using const_reference = value_type;
      ///
      using reference = const_reference;
      ///
      using const_pointer = const value_type *;
      ///
      using pointer = const_pointer;
      ///
      using difference_type = std::ptrdiff_t;
      ///
      using size_type = std::size_t;

      ///
      constexpr explicit percent_decode_iterator(std::string_view s) : remainder_(s) {}

      ///
      /// \return
      constexpr auto operator++(int) noexcept -> percent_decode_iterator {
        assert(!remainder_.empty());
        auto result = *this;
        increment();
        return result;
      }

      ///
      /// \return
      constexpr auto operator++() noexcept -> percent_decode_iterator & {
        assert(!remainder_.empty());
        increment();
        return *this;
      }

      ///
      /// \return
      [[nodiscard]] constexpr auto operator*() const noexcept -> const_reference {
        assert(!remainder_.empty());

        if (remainder_[0] == '%') {
          if (remainder_.size() < 3) {
            return tl::make_unexpected(percent_encoding::percent_encode_errc::overflow);
          }
          auto v0 = details::alnum_to_hex(remainder_[1]);
          auto v1 = details::alnum_to_hex(remainder_[2]);

          if (!v0 || !v1) {
            return tl::make_unexpected(percent_encoding::percent_encode_errc::non_hex_input);
          }

          return static_cast<char>((0x10u * std::to_integer<unsigned int>(v0.value())) +
                                  std::to_integer<unsigned int>(v1.value()));
        } else {
          return remainder_[0];
        }
      }

      ///
      /// \param sentinel
      /// \return
      constexpr auto operator==([[maybe_unused]] sentinel sentinel) const noexcept -> bool {
        return remainder_.empty();
      }

      ///
      /// \param sentinel
      /// \return
      constexpr auto operator!=(sentinel sentinel) const noexcept -> bool {
        return !(*this == sentinel);
      }

    private:
      constexpr void increment() {
        auto step = (remainder_[0] == '%') ? 3u : 1u;
        remainder_.remove_prefix(step);
      }

      std::string_view remainder_;
    };

    ///
    class percent_decode_range {
      using iterator_type = percent_decode_iterator;

    public:
      ///
      using const_iterator = iterator_type;
      ///
      using iterator = const_iterator;
      ///
      using size_type = std::size_t;

      ///
      /// \param range
      constexpr explicit percent_decode_range(std::string_view s) : it_(s) {
      }

      ///
      /// \return
      [[nodiscard]] constexpr auto cbegin() const noexcept {
        return it_;
      }

      ///
      /// \return
      [[nodiscard]] constexpr auto cend() const noexcept {
        return sentinel{};
      }

      ///
      /// \return
      [[nodiscard]] constexpr auto begin() const noexcept {
        return cbegin();
      }

      ///
      /// \return
      [[nodiscard]] constexpr auto end() const noexcept {
        return cend();
      }

      ///
      /// \return
      [[nodiscard]] constexpr auto empty() const noexcept {
        return begin() == end();
      }

    private:
      iterator_type it_;
    };


    /// Percent decodes the input
    /// \returns The percent decoded output when successful, an error otherwise.
    inline auto percent_decode(std::string_view input) -> tl::expected<std::string, percent_encode_errc> {
      auto result = std::string{};
      for (auto &&value : percent_decode_range{input}) {
        if (!value) {
          return tl::make_unexpected((value).error());
        }
        result.push_back((value).value());
      }
      return result;
    }
  }  // namespace skyr::inline v3::percent_encoding
}  // export
