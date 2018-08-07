// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/details/percent_decode.hpp>


TEST(percent_decode, decode_codepoints_before_0x80_set) {
  for (auto i = 0; i < 0x80; ++i) {
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02X", i);
    auto input = std::string("%") + buffer;
    auto decoded = skyr::details::pct_decode_char(input);
    ASSERT_TRUE(decoded);
    EXPECT_EQ(static_cast<char>(i), decoded.value());
  }
}
