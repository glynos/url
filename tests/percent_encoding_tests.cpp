// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/percent_encode.hpp>

class encode_fragment_tests : public ::testing::TestWithParam<char> {};

INSTANTIATE_TEST_CASE_P(
    encode_fragment,
    encode_fragment_tests,
    ::testing::Values(' ', '\"', '<', '>', '`'));

TEST_P(encode_fragment_tests, encode_fragment_set) {
  auto encoded = skyr::percent_encode_byte(GetParam(), skyr::fragment_set());
  ASSERT_TRUE(skyr::is_percent_encoded(encoded));
}

class encode_path_tests : public ::testing::TestWithParam<char> {};

INSTANTIATE_TEST_CASE_P(
    encode_path,
    encode_path_tests,
    ::testing::Values(' ', '\"', '<', '>', '`', '#', '?', '{', '}'));

TEST_P(encode_path_tests, encode_path_set) {
  auto encoded = skyr::percent_encode_byte(GetParam(), skyr::path_set());
  ASSERT_TRUE(skyr::is_percent_encoded(encoded));
}

class encode_userinfo_tests : public ::testing::TestWithParam<char> {};

INSTANTIATE_TEST_CASE_P(
    encode_userinfo,
    encode_userinfo_tests,
    ::testing::Values(
        ' ', '\"', '<', '>', '`', '#', '?', '{', '}', '/', ':', ';', '=', '@', '[', '\\', ']', '^', '|'));

TEST_P(encode_userinfo_tests, encode_userinfo_set) {
  auto encoded = skyr::percent_encode_byte(GetParam(), skyr::userinfo_set());
  ASSERT_TRUE(skyr::is_percent_encoded(encoded));
}

TEST(encode_tests, encode_codepoints_before_0x20_set) {
  for (auto i = 0; i < 0x20; ++i) {
    auto encoded = skyr::percent_encode_byte(static_cast<char>(i));
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02X", i);
    auto output = std::string("%") + buffer;
    EXPECT_EQ(output, encoded);
  }
}

TEST(encode_tests, encode_codepoints_before_0x7e_set) {
  for (auto i = 0x7f; i <= 0xff; ++i) {
    auto encoded = skyr::percent_encode_byte(static_cast<char>(i));
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02X", i);
    auto output = std::string("%") + buffer;
    ASSERT_EQ(output, encoded);
  }
}

TEST(encode_tests, u8_test_1) {
  auto input = std::string("\xf0\x9f\x92\xa9");
  auto encoded = skyr::percent_encode(input);
  ASSERT_TRUE(encoded);
  EXPECT_EQ("%F0%9F%92%A9", encoded.value());
}

TEST(encode_tests, u8_test_2) {
  auto input = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
  auto encoded = skyr::percent_encode(input);
  ASSERT_TRUE(encoded);
  EXPECT_EQ("%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88", encoded.value());
}

TEST(encode_tests, DISABLED_u32_test_1) {
  auto input = std::u32string(U"\u1F4A9");
  auto encoded = skyr::percent_encode(input);
  ASSERT_TRUE(encoded);
  EXPECT_EQ("%F0%9F%92%A9", encoded.value());
}

TEST(encode_tests, DISABLED_u32_test_2) {
  auto input = std::u32string(U"\u1F3F3\uFE0F\200D\1F308");
  auto encoded = skyr::percent_encode(input);
  ASSERT_TRUE(encoded);
  EXPECT_EQ("%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88", encoded.value());
}
