// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "skyr/domain.hpp"


using param = std::pair<std::string, std::string>;

class punycode_tests : public ::testing::TestWithParam<param> {};

INSTANTIATE_TEST_CASE_P(
    encode_tests,
    punycode_tests,
    ::testing::Values(
        param{"你好你好", "xn--6qqa088eba"},
        param{"你", "xn--6qq"},
        param{"好", "xn--5us"},
        param{"你好", "xn--6qq79v"},
        param{"你好你", "xn--6qqa088e"},
        param{"點看", "xn--c1yn36f"},
        param{"faß", "xn--fa-hia"},
        param{"☃", "xn--n3h"},
        param{"bücher", "xn--bcher-kva"},
        param("ü", "xn--tda"),
        param("⌘", "xn--bih"),
        param("ñ", "xn--ida"),
        param("☃", "xn--n3h"),
        param{"उदाहरण", "xn--p1b6ci4b4b3a"},
        param{"परीक्षा", "xn--11b5bs3a9aj6g"},
        param("glyn", "xn--glyn-")
    ));

TEST_P(punycode_tests, encode_set) {
  auto input = std::string();
  auto expected = std::string();

  std::tie(input, expected) = GetParam();
  auto encoded = skyr::punycode_encode(input);
  ASSERT_TRUE(encoded);
  EXPECT_EQ(expected, encoded.value())
    << input << " --> " << expected << "(" << encoded.value() << ")";
}

TEST_P(punycode_tests, decode_set) {
  auto input = std::string();
  auto expected = std::string();

  std::tie(expected, input) = GetParam();
  auto decoded = skyr::punycode_decode(input);
  ASSERT_TRUE(decoded);
  EXPECT_EQ(expected, decoded.value())
    << input << " --> " << expected << "(" << decoded.value() << ")";
}
