// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/ipv6_address.hpp>


TEST(ipv6_address_tests, zero_test) {
  auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
  auto instance = skyr::ipv6_address(address);
  EXPECT_EQ("[::]", instance.to_string());
}

TEST(ipv6_address_tests, loopback_test) {
  auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 1}};
  auto instance = skyr::ipv6_address(address);
  EXPECT_EQ("[::1]", instance.to_string());
}
