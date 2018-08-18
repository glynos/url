// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/percent_encode.hpp>


TEST(percent_decode, decode_codepoints_set) {
  for (auto i = 0; i < 0xff; ++i) {
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02X", i);
    auto input = std::string("%") + buffer;
    auto decoded = skyr::pct_decode_byte(input);
    ASSERT_TRUE(decoded);
    EXPECT_EQ(static_cast<char>(i), decoded.value());
  }
}

TEST(percent_decode_tests, u8_test) {
  auto input = std::string("%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88");
  auto decoded = skyr::pct_decode(input);
  ASSERT_TRUE(decoded);
  EXPECT_EQ("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", decoded);
}