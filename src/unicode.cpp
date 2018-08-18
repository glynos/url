// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <codecvt>
#include <locale>
#include "skyr/unicode.hpp"

namespace skyr {
namespace {
class unicode_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *unicode_error_category::name() const noexcept {
  return "unicode";
}

std::string unicode_error_category::message(int error) const noexcept {
  switch (static_cast<unicode_errc>(error)) {
    case unicode_errc::overflow:
      return "Overflow";
    default:
      return "(Unknown error)";
  }
}

static const unicode_error_category category{};
}  // namespace

std::error_code make_error_code(unicode_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

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

expected<std::string, std::error_code> wstring_to_bytes(wstring_view input) {
  try {
    return wstring().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::wstring, std::error_code> wstring_from_bytes(string_view input) {
  try {
    return wstring().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::u16string, std::error_code> ucs2_from_bytes(string_view input) {
  try {
    return ucs2().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::string, std::error_code> ucs2_to_bytes(u16string_view input) {
  try {
    return ucs2().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::u32string, std::error_code> ucs4_from_bytes(string_view input) {
  try {
    return ucs4().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::string, std::error_code> ucs4_to_bytes(u32string_view input) {
  try {
    return ucs4().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}
}  // namespace skyr
