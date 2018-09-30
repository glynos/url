// Copyright (c) Glyn Matthews 2012-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_DETAILS_TRANSLATE_INC
#define SKYR_URL_DETAILS_TRANSLATE_INC

#include <string>
#include <skyr/unicode.hpp>

namespace skyr {
namespace details {
template <typename Source>
struct to_bytes_impl;

template <>
struct to_bytes_impl<std::string> {
  expected<std::string, std::error_code> operator()(const std::string &source) const { return source; }
};

template <int N>
struct to_bytes_impl<char[N]> {
  expected<std::string, std::error_code> operator()(const char *source) const { return source; }
};

template <>
struct to_bytes_impl<char *> {
  expected<std::string, std::error_code> operator()(const char *source) const { return source; }
};

template <>
struct to_bytes_impl<const char *> {
  expected<std::string, std::error_code> operator()(const char *source) const { return source; }
};

template <int N>
struct to_bytes_impl<const char[N]> {
  expected<std::string, std::error_code> operator()(const char *source) const { return source; }
};

template <>
struct to_bytes_impl<std::wstring> {
  expected<std::string, std::error_code> operator()(const std::wstring &source) const {
    return wstring_to_bytes(source);
  }
};

template <int N>
struct to_bytes_impl<const wchar_t[N]> {
  expected<std::string, std::error_code> operator()(const wchar_t *source) const {
    to_bytes_impl<std::wstring> to_bytes;
    return to_bytes(source);
  }
};

template <int N>
struct to_bytes_impl<wchar_t[N]> {
  expected<std::string, std::error_code> operator()(const wchar_t *source) const {
    to_bytes_impl<std::wstring> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<wchar_t *> {
  expected<std::string, std::error_code> operator()(const wchar_t *source) const {
    to_bytes_impl<std::wstring> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<const wchar_t *> {
  expected<std::string, std::error_code> operator()(const wchar_t *source) const {
    to_bytes_impl<std::wstring> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<std::u16string> {
  expected<std::string, std::error_code> operator()(const std::u16string &source) const {
    return utf16_to_bytes(source);
  }
};

template <int N>
struct to_bytes_impl<const char16_t[N]> {
  expected<std::string, std::error_code> operator()(const char16_t *source) const {
    to_bytes_impl<std::u16string> to_bytes;
    return to_bytes(source);
  }
};

template <int N>
struct to_bytes_impl<char16_t[N]> {
  expected<std::string, std::error_code> operator()(const char16_t *source) const {
    to_bytes_impl<std::u16string> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<char16_t *> {
  expected<std::string, std::error_code> operator()(const char16_t *source) const {
    to_bytes_impl<std::u16string> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<const char16_t *> {
  expected<std::string, std::error_code> operator()(const char16_t *source) const {
    to_bytes_impl<std::u16string> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<std::u32string> {
  expected<std::string, std::error_code> operator()(const std::u32string &source) const {
    return utf32_to_bytes(source);
  }
};

template <int N>
struct to_bytes_impl<const char32_t[N]> {
  expected<std::string, std::error_code> operator()(const char32_t *source) const {
    to_bytes_impl<std::u32string> to_bytes;
    return to_bytes(source);
  }
};

template <int N>
struct to_bytes_impl<char32_t[N]> {
  expected<std::string, std::error_code> operator()(const char32_t *source) const {
    to_bytes_impl<std::u32string> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<char32_t *> {
  expected<std::string, std::error_code> operator()(const char32_t *source) const {
    to_bytes_impl<std::u32string> to_bytes;
    return to_bytes(source);
  }
};

template <>
struct to_bytes_impl<const char32_t *> {
  expected<std::string, std::error_code> operator()(const char32_t *source) const {
    to_bytes_impl<std::u32string> to_bytes;
    return to_bytes(source);
  }
};

template <typename Source>
inline expected<std::string, std::error_code> to_bytes(const Source &source) {
  to_bytes_impl<Source> to_bytes;
  return to_bytes(source);
}
}  // namespace details
}  // namespace skyr

#endif  // SKYR_URL_DETAILS_TRANSLATE_INC
