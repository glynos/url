// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <codecvt>
#include <locale>
#include <clocale>
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
    case unicode_errc::illegal_byte_sequence:
      return "Illegal byte sequence";
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
using wstring_convert = std::wstring_convert<std::codecvt_utf8<wchar_t>>;
using utf16_convert = std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>;
using utf32_convert = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;

static wstring_convert &wstring() {
  static wstring_convert conv;
  return conv;
}

static utf16_convert &utf16() {
  static utf16_convert conv;
  return conv;
}

static utf32_convert &utf32() {
  static utf32_convert conv;
  return conv;
}
}  // namespace

expected<std::string, std::error_code> wstring_to_bytes(std::wstring_view input) {
  try {
    return wstring().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::wstring, std::error_code> wstring_from_bytes(std::string_view input) {
  try {
    return wstring().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::string, std::error_code> utf16_to_bytes(std::u16string_view input) {
  try {
    return utf16().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::u16string, std::error_code> utf16_from_bytes(std::string_view input) {
  try {
    return utf16().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::u32string, std::error_code> utf32_from_bytes(std::string_view input) {
  try {
    return utf32().from_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::string, std::error_code> utf32_to_bytes(std::u32string_view input) {
  try {
    return utf32().to_bytes(begin(input), end(input));
  }
  catch (std::range_error &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}
}  // namespace skyr
