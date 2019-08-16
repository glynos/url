// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <skyr/url/domain.hpp>

using param = std::pair<std::string, std::string>;

class domain_tests : public ::testing::TestWithParam<param> {};

INSTANTIATE_TEST_SUITE_P(
    domain_to_ascii_tests,
    domain_tests,
    ::testing::Values(
        param{"example.com", "example.com"},
        param{"⌘.ws", "xn--bih.ws"},
        param{"你好你好", "xn--6qqa088eba"},
        param{"你好你好.com", "xn--6qqa088eba.com"},
        param{"उदाहरण.परीक्षा", "xn--p1b6ci4b4b3a.xn--11b5bs3a9aj6g"},
        param{"faß.ExAmPlE", "xn--fa-hia.example"},
        param{"βόλος.com", "xn--nxasmm1c.com"},
        param{"Ｇｏ.com", "go.com"}
    ));

TEST_P(domain_tests, domain_to_ascii_tests) {
  auto input = std::string();
  auto expected = std::string();
  std::tie(input, expected) = GetParam();

  auto instance = skyr::domain_to_ascii(input);
  ASSERT_TRUE(instance);
  EXPECT_EQ(expected, instance.value());
}

TEST(domain_tests, invalid_domain_1) {
  auto instance = skyr::domain_to_ascii("GOO 　goo.com");
  ASSERT_FALSE(instance);
}

TEST(domain_tests, invalid_domain_2) {
  auto instance = skyr::domain_to_ascii(U"\xfdD0zyx.com");
  ASSERT_FALSE(instance);
}

TEST(domain_tests, invalid_domain_3) {
  auto instance = skyr::domain_to_ascii("�");
  ASSERT_FALSE(instance);
}

TEST(domain_tests, invalid_domain_4) {
  auto instance = skyr::domain_to_ascii("％４１.com");
  ASSERT_FALSE(instance);
}
