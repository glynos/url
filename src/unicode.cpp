// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <codecvt>
#include <locale>
#include "skyr/unicode.hpp"

namespace skyr {
namespace {
static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> &ucs4_convert() {
  static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> ucs4conv;
  return ucs4conv;
}
}  // namespace

expected<std::u32string, unicode_errc> ucs4_from_bytes(string_view input) {
  try {
    return ucs4_convert().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}

expected<std::string, unicode_errc> ucs4_to_bytes(u32string_view input) {
  try {
    return ucs4_convert().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(unicode_errc::range_error);
  }
}
}  // namespace skyr
