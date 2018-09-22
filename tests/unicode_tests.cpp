// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include "skyr/unicode.hpp"


TEST(unicode_tests, utf32_to_bytes_poo_emoji_test) {
  auto input = std::u32string(U"\x1F4A9");
  auto bytes = skyr::utf32_to_bytes(input);
  ASSERT_TRUE(bytes);
  EXPECT_EQ("\xf0\x9f\x92\xa9", bytes.value());
}

TEST(unicode_tests, bytes_to_utf32_poo_emoji_test) {
  auto input = std::string("\xf0\x9f\x92\xa9");
  auto utf32 = skyr::utf32_from_bytes(input);
  ASSERT_TRUE(utf32);
  EXPECT_EQ(U"\x1F4A9", utf32.value());
}

TEST(unicode_tests, utf32_rainbow_flag_test) {
  auto input = std::u32string(U"\x1F3F3\xFE0F\x200D\x1F308");
  auto bytes = skyr::utf32_to_bytes(input);
  ASSERT_TRUE(bytes);
  EXPECT_EQ("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", bytes.value());
}
