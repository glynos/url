// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include "skyr/unicode.hpp"


TEST(unicode_tests, DISABLED_ucs4_to_bytes_poo_emoji_test) {
  auto input = std::u32string(U"\u1F4A9");
  auto bytes = skyr::utf32_to_bytes(input);
  ASSERT_TRUE(bytes);
  EXPECT_EQ("\xf0\x9f\x92\xa9", bytes.value());
}

TEST(unicode_tests, DISABLED_bytes_to_ucs4_poo_emoji_test) {
  auto input = std::string("\xf0\x9f\x92\xa9");
  auto ucs4 = skyr::utf32_from_bytes(input);
  ASSERT_TRUE(ucs4);
  EXPECT_EQ(U"\u1F4A9", ucs4.value());
}

TEST(unicode_tests, DISABLED_ucs4_rainbow_flag_test) {
  auto input = std::u32string(U"\u1F3F3\uFE0F\200D\1F308");
  auto bytes = skyr::utf32_to_bytes(input);
  ASSERT_TRUE(bytes);
  EXPECT_EQ("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", bytes.value());
}
