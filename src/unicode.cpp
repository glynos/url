// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <codecvt>
#include <locale>
#include "skyr/unicode.hpp"

namespace skyr {
namespace {
using wstring_convert = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>;
using ucs2_convert = std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>;
using ucs4_convert = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;

static wstring_convert &wstring() {
  static wstring_convert conv;
  return conv;
}

static ucs2_convert &ucs2() {
  static ucs2_convert conv;
  return conv;
}

static ucs4_convert &ucs4() {
  static ucs4_convert conv;
  return conv;
}
}  // namespace

expected<std::string, unicode_errc> wstring_to_bytes(wstring_view input) {
  try {
    return wstring().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}

expected<std::u16string, unicode_errc> ucs2_from_bytes(string_view input) {
  try {
    return ucs2().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}

expected<std::string, unicode_errc> ucs2_to_bytes(u16string_view input) {
  try {
    return ucs2().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}

expected<std::u32string, unicode_errc> ucs4_from_bytes(string_view input) {
  try {
    return ucs4().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}

expected<std::string, unicode_errc> ucs4_to_bytes(u32string_view input) {
  try {
    return ucs4().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}
}  // namespace skyr
