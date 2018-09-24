// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PERCENT_ENCODE_INC
#define SKYR_URL_PERCENT_ENCODE_INC

#include <string>
#include <string_view>
#include <locale>
#include <set>
#include <cstddef>
#include <skyr/expected.hpp>

namespace skyr {
/// Enumerates percent encoding errors
enum class percent_encode_errc {
  /// Input was not a hex value
  non_hex_input,
  /// Overflow
  overflow,
};

/// Creates a `std::error_code` given a `skyr::percent_encode_errc`
/// value
/// \param error A percent encoding error
/// \returns A `std::error_code` object
std::error_code make_error_code(percent_encode_errc error);

/// Exclude code point set when percent encoding
class exclude_set {
 protected:
  virtual ~exclude_set() {}

 public:
  /// Tests whether the byte is in the excluded set
  /// \param byte Input byte
  /// \returns `true` if `in` is in the excluded set, `false`
  ///          otherwise
  bool contains(char byte) const {
    return contains_impl(byte);
  }

 private:
  virtual bool contains_impl(char byte) const = 0;
};

/// Defines code points in the c0 control percent-encode set
class c0_control_set : public exclude_set {
 public:
  virtual ~c0_control_set() {}

 private:
  bool contains_impl(char byte) const override {
    return (byte <= 0x1f) || (byte > 0x7e);
  }
};

/// Defines code points in the fragment percent-encode set
class fragment_set : public exclude_set {
 public:
  fragment_set() : set_{0x20, 0x22, 0x3c, 0x3e, 0x60} {}
  virtual ~fragment_set() {}

 private:
  bool contains_impl(char byte) const override {
    return
        c0_control_set_.contains(byte) ||
        (set_.find(byte) != set_.end());
  }

 private:
  c0_control_set c0_control_set_;
  std::set<char> set_;
};

/// Defines code points in the fragment percent-encode set and
/// U+0027 (')
class query_set : public exclude_set {
 public:
  virtual ~query_set() {}

 private:
  bool contains_impl(char byte) const override {
    return
        fragment_set_.contains(byte) || (byte == 0x27);
  }

 private:
  fragment_set fragment_set_;
};

/// Defines code points in the path percent-encode set
class path_set : public exclude_set {
 public:
  path_set() : set_{0x23, 0x3f, 0x7b, 0x7d} {}
  virtual ~path_set() {}

 private:
  bool contains_impl(char byte) const override {
    return
        fragment_set_.contains(byte) ||
        (set_.find(byte) != set_.end());
  }

 private:
  fragment_set fragment_set_;
  std::set<char> set_;
};

/// Defines code points in the userinfo percent-encode set
class userinfo_set : public exclude_set {
 public:
  userinfo_set()
    : set_{0x2f, 0x3a, 0x3b, 0x3d, 0x40, 0x5b, 0x5c, 0x5d, 0x5e, 0x7c} {}
  virtual ~userinfo_set() {}

 private:
  bool contains_impl(char byte) const override {
    return
        path_set_.contains(byte) ||
        (set_.find(byte) != set_.end());
  }

 private:
  path_set path_set_;
  std::set<char> set_;
};

/// Percent encodes a byte if it is not in the exclude set
/// \param byte The input byte
/// \param excludes The set of code points to exclude when percent
///        encoding
/// \returns A percent encoded string if `in` is not in the
///          exclude set, `in` as a string otherwise
std::string percent_encode_byte(
    char byte, const exclude_set &excludes = c0_control_set());

/// Percent encodes a string
/// \param input A string of bytes
/// \param excludes The set of code points to exclude when percent
///        encoding
/// \returns A percent encoded ASCII string, or an error on failure
expected<std::string, std::error_code> percent_encode(
    std::string_view input, const exclude_set &excludes = c0_control_set());

/// Percent encodes a string
/// \param input A UTF-32 string
/// \param excludes The set of code points to exclude when percent
///        encoding
/// \returns A percent encoded ASCII string, or an error on failure
expected<std::string, std::error_code> percent_encode(
  std::u32string_view input, const exclude_set &excludes = c0_control_set());

/// Percent decode an already encoded string into a byte value
/// \param input An string of the for %XX, where X is a hexadecimal
///        value
/// \returns The percent decoded byte, or an error on failure
expected<char, std::error_code> percent_decode_byte(
    std::string_view input);

/// Percent decodes a string
/// \param input An ASCII string
/// \returns A UTF-8 string, or an error on failure
expected<std::string, std::error_code> percent_decode(
    std::string_view input);

/// Tests whether the input string contains percent encoded values
/// \param input An ASCII string
/// \param locale A locale
/// \returns `true` if the input string contains percent encoded
///          values, `false` otherwise
bool is_percent_encoded(
    std::string_view input,
    const std::locale &locale = std::locale::classic());
}  // namespace skyr

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
namespace std {
template <>
struct is_error_code_enum<skyr::percent_encode_errc> : true_type {};
}  // namespace std
#endif  // !defined(DOXYGEN_SHOULD_SKIP_THIS)

#endif  // SKYR_URL_PERCENT_ENCODE_INC
