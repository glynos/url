// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <skyr/url/details/to_bytes.hpp>

TEST(byte_conversion_tests, weird_01) {
  auto bytes = skyr::details::to_bytes(U"http://\xfdD0zyx.com");
  EXPECT_TRUE(bytes);
}

TEST(byte_conversion_tests, weird_02) {
  auto bytes = skyr::details::to_bytes("/\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
  EXPECT_TRUE(bytes);
}
