// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_PERCENT_ENCODE_INC
#define SKYR_URL_PERCENT_ENCODE_INC

#include <string>
#include <string_view>
#include <locale>
#include <skyr/expected.hpp>

namespace skyr {
/// Enumerates percent encoding errors.
enum class percent_encode_errc {
  /// Input was not a hex value.
  non_hex_input,
  /// Overflow.
  overflow,
};

/// Creates a `std::error_code` given a `skyr::percent_encode_errc` value.
/// \param error A percent encoding error.
/// \returns A `std::error_code` object.
std::error_code make_error_code(percent_encode_errc error);

/// Exclude code point set when percent encoding.
class exclude_set {
 protected:
  virtual ~exclude_set() {}

 public:
  /// Tests whether the byte is in the excluded set.
  /// \param in Input byte.
  /// \returns `true` if `in` is in the excluded set, `c` false otherwise.
  virtual bool is_excluded(char in) const = 0;
};

/// Defines code points in the c0 control percent-encode set.
class c0_control_set : public exclude_set {
 public:
  virtual ~c0_control_set() {}

  bool is_excluded(char in) const override {
    return (in <= 0x1f) || (in > 0x7e);
  }
};

/// Defines code points in the fragment percent-encode set.
class fragment_set : public exclude_set {
 public:
  virtual ~fragment_set() {}

  bool is_excluded(char in) const override {
    return
      c0_control_set_.is_excluded(in) || (in == 0x20) || (in == 0x22) || (in == 0x3c) || (in == 0x3e) || (in == 0x60);
  }

 private:
  c0_control_set c0_control_set_;
};

/// Defines code points in the fragment percent-encode set and U+0027 (').
class query_set : public exclude_set {
 public:
  virtual ~query_set() {}

  bool is_excluded(char in) const override {
    return
      fragment_set_.is_excluded(in) || (in == 0x27);
  }

 private:
  fragment_set fragment_set_;
};

/// Defines code points in the path percent-encode set.
class path_set : public exclude_set {
 public:
  virtual ~path_set() {}

  bool is_excluded(char in) const override {
    return
        fragment_set_.is_excluded(in) || (in == 0x23) || (in == 0x3f) || (in == 0x7b) || (in == 0x7d);
  }

 private:
  fragment_set fragment_set_;
};

/// Defines code points in the userinfo percent-encode set.
class userinfo_set : public exclude_set {
 public:
  virtual ~userinfo_set() {}

  bool is_excluded(char in) const override {
    return
    path_set_.is_excluded(in) || (in == 0x2f) || (in == 0x3a) || (in == 0x3b) || (in == 0x3d) ||
        (in == 0x40) || (in == 0x5b) || (in == 0x5c) || (in == 0x5d) || (in == 0x5e) || (in == 0x7c);
  }

 private:
  path_set path_set_;
};

/// Percent encode a byte if it is not in the exclude set.
/// \param in The input byte.
/// \param excludes The set of code points to exclude when percent encoding.
/// \returns A percent encoded string if `in` is not in the exclude set, `in` as a string otherwise.
std::string percent_encode_byte(
    char in, const exclude_set &excludes = c0_control_set());

/// Percent encode a string.
/// \param input A string of bytes.
/// \param excludes The set of code points to exclude when percent encoding.
/// \returns A percent encoded ASCII string, or an error on failure.
expected<std::string, std::error_code> percent_encode(
    std::string_view input, const exclude_set &excludes = c0_control_set());

/// Percent encode a string.
/// \param input A UTF-32 string.
/// \param excludes The set of code points to exclude when percent encoding.
/// \returns A percent encoded ASCII string, or an error on failure.
expected<std::string, std::error_code> percent_encode(
  std::u32string_view input, const exclude_set &excludes = c0_control_set());

/// Percent decode an already encoded string into a byte value.
/// \param input An string of the for %XX, where X is a hexadecimal value/
/// \returns The percent decoded byte, or an error on failure.
expected<char, std::error_code> percent_decode_byte(std::string_view input);

/// Percent decodes a string.
/// \param input An ASCII string.
/// \returns A UTF-8 string, or an error on failure.
expected<std::string, std::error_code> percent_decode(std::string_view input);

/// Tests whether the input string contains percent encoded values.
/// \param input A string.
/// \param locale A locale.
/// \returns `true` if the input string contains percent encoded values, `false` otherwise.
bool is_percent_encoded(
    std::string_view input,
    const std::locale &locale = std::locale::classic());
}  // namespace skyr

/// \exclude
namespace std {
template <>
struct is_error_code_enum<skyr::percent_encode_errc> : true_type {};
}  // namespace std

#endif  // SKYR_URL_PERCENT_ENCODE_INC
