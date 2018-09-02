// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include "skyr/unicode.hpp"
#include "utf8/checked.h"

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

expected<std::u32string, std::error_code> utf32_from_bytes(std::string_view input) {
  try {
    std::u32string result;
    utf8::utf8to32(begin(input), end(input),std::back_inserter(result));
    return result;
  }
  catch (utf8::exception &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}

expected<std::string, std::error_code> utf32_to_bytes(std::u32string_view input) {
  try {
    std::string result;
    utf8::utf32to8(begin(input), end(input), std::back_inserter(result));
    return result;
  }
  catch (utf8::exception &) {
    return make_unexpected(make_error_code(unicode_errc::overflow));
  }
}
}  // namespace skyr
