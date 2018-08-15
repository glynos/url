// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_HPP
#define SKYR_UNICODE_HPP

#include <string>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
enum class unicode_errc {
  overflow,
};

/// \param
/// \returns
expected<std::string, unicode_errc> wstring_to_bytes(wstring_view input);

/// \param
/// \returns
expected<std::u16string, unicode_errc> ucs2_from_bytes(string_view input);

/// \param
/// \returns
expected<std::string, unicode_errc> ucs2_to_bytes(u16string_view input);

/// \param
/// \returns
expected<std::u32string, unicode_errc> ucs4_from_bytes(string_view input);

/// \param
/// \returns
expected<std::string, unicode_errc> ucs4_to_bytes(u32string_view input);
}  // namespace skyr

#endif //SKYR_UNICODE_HPP
