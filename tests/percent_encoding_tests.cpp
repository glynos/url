// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/details/encode.hpp>


class encode_fragment_tests : public ::testing::TestWithParam<char> {};

INSTANTIATE_TEST_CASE_P(
    encode_fragment,
    encode_fragment_tests,
    ::testing::Values(' ', '\"', '<', '>', '`'));

TEST_P(encode_fragment_tests, encode_fragment_set) {
  auto encoded = skyr::details::pct_encode_char(GetParam(), " \"<>`");
  auto view = skyr::string_view(encoded);
  ASSERT_TRUE(skyr::details::is_pct_encoded(begin(view), end(view)));
}

class encode_path_tests : public ::testing::TestWithParam<char> {};

INSTANTIATE_TEST_CASE_P(
    encode_path,
    encode_path_tests,
    ::testing::Values(' ', '\"', '<', '>', '`', '#', '?', '{', '}'));

TEST_P(encode_path_tests, encode_path_set) {
  auto encoded = skyr::details::pct_encode_char(GetParam(), " \"<>`#?{}");
  auto view = skyr::string_view(encoded);
  ASSERT_TRUE(skyr::details::is_pct_encoded(begin(view), end(view)));
}

class encode_userinfo_tests : public ::testing::TestWithParam<char> {};

INSTANTIATE_TEST_CASE_P(
    encode_userinfo,
    encode_userinfo_tests,
    ::testing::Values(
        ' ', '\"', '<', '>', '`', '#', '?', '{', '}', '/', ':', ';', '=', '@', '[', '\\', ']', '^', '|'));

TEST_P(encode_userinfo_tests, encode_userinfo_set) {
  auto encoded = skyr::details::pct_encode_char(GetParam(), " \"<>`#?{}/:;=@[\\]^|");
  auto view = skyr::string_view(encoded);
  ASSERT_TRUE(skyr::details::is_pct_encoded(begin(view), end(view)));
}

TEST(encode_tests, encode_codepoints_before_0x20_set) {
  for (auto i = 0; i < 0x20; ++i) {
    auto encoded = skyr::details::pct_encode_char(static_cast<char>(i));
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02X", i);
    auto output = std::string("%") + buffer;
    EXPECT_EQ(output, encoded);
  }
}

TEST(encode_tests, encode_codepoints_before_0x7e_set) {
  for (auto i = 0x7f; i <= 0xff; ++i) {
    auto encoded = skyr::details::pct_encode_char(static_cast<char>(i));
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02X", i);
    auto output = std::string("%") + buffer;
    ASSERT_EQ(output, encoded);
  }
}
