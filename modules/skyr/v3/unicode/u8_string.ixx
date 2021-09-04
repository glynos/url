// Copyright 2012-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <string>
#include <tl/expected.hpp>

export module skyr.v3.unicode.u8_string;

export import skyr.v3.unicode.unicode_errc;
export import skyr.v3.concepts.url_concepts;
export import skyr.v3.unicode.ranges.transforms.transform_u8;
export import skyr.v3.unicode.ranges.views.view_u16;

export {
  namespace skyr::inline v3::unicode {
  template <class Source>
  requires is_string_container<Source, char>
  inline auto to_u8(const Source &source) -> tl::expected<std::string, unicode::unicode_errc> {
    return std::string(source);
  }

  template <class Source>
  requires is_string_container<Source, char8_t>
  inline auto to_u8(const Source &source) -> tl::expected<std::string, unicode::unicode_errc> {
    return std::string(std::cbegin(source), std::cend(source));
  }

  template <class Source>
  requires is_string_container<Source, char16_t> || is_string_container<Source, wchar_t>
  inline auto to_u8(const Source &source) -> tl::expected<std::string, unicode::unicode_errc> {
    return unicode::as<std::string>(unicode::views::as_u16(source) | unicode::transforms::to_u8);
  }

  template <class Source>
  requires is_string_container<Source, char32_t>
  inline auto to_u8(const Source &source) -> tl::expected<std::string, unicode::unicode_errc> {
    return unicode::as<std::string>(source | unicode::transforms::to_u8);
  }
  }  // namespace skyr::inline v3::unicode
}
