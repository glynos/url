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
template <class T, class charT>
using is_basic_string =
    std::is_same<typename std::remove_cv<T>::type, std::basic_string<charT>>;

template <class T, class charT>
using is_basic_string_view =
    std::is_same<typename std::remove_cv<T>::type, std::basic_string_view<charT>>;

template <class T, class charT>
using is_char_array =
    std::conjunction<
        std::is_array<T>,
        std::is_same<typename std::remove_cv<typename std::remove_extent<T>::type>::type, charT>>;

template <class T, class charT>
using is_char_pointer =
    std::conjunction<
        std::is_pointer<T>,
        std::is_same<typename std::remove_pointer<T>::type, charT>>;

template <class T, class charT>
using is_string_convertible =
    std::disjunction<
        is_basic_string<T, charT>,
        is_basic_string_view<T, charT>,
        is_char_array<T, charT>,
        is_char_pointer<T, charT>>
        ;

template <class T>
using is_any_string_convertible =
    std::disjunction<
        is_string_convertible<T, char>,
        is_string_convertible<T, wchar_t>,
        is_string_convertible<T, char16_t>,
        is_string_convertible<T, char32_t>>;

template <class Source, class Enable = void>
struct to_bytes_impl;

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return source;
  }
};

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, wchar_t>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return wstring_to_bytes(source);
  }
};

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char16_t>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return utf16_to_bytes(source);
  }
};

template <class Source>
struct to_bytes_impl<
    Source, typename std::enable_if<is_string_convertible<Source, char32_t>::value>::type> {
  expected<std::string, std::error_code> operator()(const Source &source) const {
    return utf32_to_bytes(source);
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
