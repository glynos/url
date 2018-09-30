// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/url.hpp>

TEST(url_setter_tests, test_href_1) {
  auto instance = skyr::url{};

  auto result = instance.set_href("http://example.com/");
  ASSERT_TRUE(result);
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/", instance.pathname());
}

TEST(url_setter_tests, test_href_2) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_href("https://cpp-netlib.org/?a=b#fragment");
  ASSERT_TRUE(result);
  EXPECT_EQ("https:", instance.protocol());
  EXPECT_EQ("cpp-netlib.org", instance.host());
  EXPECT_EQ("/", instance.pathname());
  EXPECT_EQ("?a=b", instance.search());
  EXPECT_EQ("#fragment", instance.hash());
}

TEST(url_setter_tests, test_href_parse_error) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_href("Ceci n'est pas un URL");
  ASSERT_FALSE(result);
  EXPECT_EQ("http:", instance.protocol());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/", instance.pathname());
  EXPECT_EQ("", instance.search());
  EXPECT_EQ("", instance.hash());
}

TEST(url_setter_tests, test_protocol_special_to_special) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_protocol("ws");
  ASSERT_TRUE(result);
  EXPECT_EQ("ws://example.com/", instance.href());
  EXPECT_EQ("ws:", instance.protocol());
}

TEST(url_setter_tests, test_protocol_special_to_non_special) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_protocol("non-special");
  ASSERT_FALSE(result);
  EXPECT_EQ("http://example.com/", instance.href());
}

TEST(url_setter_tests, test_protocol_non_special_to_special) {
  auto instance = skyr::url{"non-special://example.com/"};

  auto result = instance.set_protocol("http");
  ASSERT_FALSE(result);
  EXPECT_EQ("non-special://example.com/", instance.href());
}

TEST(url_setter_tests, test_protocol_has_port_to_file) {
  auto instance = skyr::url{"http://example.com:8080/"};

  auto result = instance.set_protocol("file");
  ASSERT_FALSE(result);
  EXPECT_EQ("http://example.com:8080/", instance.href());
}

TEST(url_setter_tests, test_protocol_has_no_port_to_file) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_protocol("file");
  ASSERT_TRUE(result);
  EXPECT_EQ("file://example.com/", instance.href());
  EXPECT_EQ("file:", instance.protocol());
}

TEST(url_setter_tests, test_username) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_username("user");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://user@example.com/", instance.href());
  EXPECT_EQ("user", instance.username());
}

TEST(url_setter_tests, test_username_pct_encoded) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_username("us er");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://us%20er@example.com/", instance.href());
  EXPECT_EQ("us%20er", instance.username());
}

TEST(url_setter_tests, test_username_file_scheme) {
  auto instance = skyr::url{"file://example.com/"};

  auto result = instance.set_username("user");
  ASSERT_FALSE(result);
  EXPECT_EQ("file://example.com/", instance.href());
}

TEST(url_setter_tests, test_password) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_password("pass");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://:pass@example.com/", instance.href());
  EXPECT_EQ("pass", instance.password());
}

TEST(url_setter_tests, test_password_pct_encoded) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_password("pa ss");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://:pa%20ss@example.com/", instance.href());
  EXPECT_EQ("pa%20ss", instance.password());
}

TEST(url_setter_tests, test_password_file_scheme) {
  auto instance = skyr::url{"file://example.com/"};

  auto result = instance.set_password("pass");
  ASSERT_FALSE(result);
  EXPECT_EQ("file://example.com/", instance.href());
}

TEST(url_setter_tests, test_host_http) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_host("elpmaxe.com");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://elpmaxe.com/", instance.href());
  EXPECT_EQ("elpmaxe.com", instance.host());
}

TEST(url_setter_tests, test_host_with_port_number) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_host("elpmaxe.com:8080");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://elpmaxe.com:8080/", instance.href());
  EXPECT_EQ("elpmaxe.com:8080", instance.host());
  EXPECT_EQ("elpmaxe.com", instance.hostname());
}

TEST(url_setter_tests, test_host_file_set_non_empty_host) {
  auto instance = skyr::url{"file:///path/to/helicon/"};

  auto result = instance.set_host("example.com");
  ASSERT_TRUE(result);
  EXPECT_EQ("file://example.com/path/to/helicon/", instance.href());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/path/to/helicon/", instance.pathname());
}

TEST(url_setter_tests, test_host_file_with_port_number) {
  auto instance = skyr::url{"file:///path/to/helicon/"};

  auto result = instance.set_host("example.com:8080");
  ASSERT_FALSE(result);
}

TEST(url_setter_tests, test_host_file_set_empty_host) {
  auto instance = skyr::url{"file://example.com/path/to/helicon/"};

  auto result = instance.set_host("");
  ASSERT_TRUE(result);
  EXPECT_EQ("file:///path/to/helicon/", instance.href());
  EXPECT_EQ("", instance.host());
  EXPECT_EQ("/path/to/helicon/", instance.pathname());
}


TEST(url_setter_tests, test_hostname_http) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_hostname("elpmaxe.com");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://elpmaxe.com/", instance.href());
  EXPECT_EQ("elpmaxe.com", instance.hostname());
}

TEST(url_setter_tests, test_hostname_with_port_number) {
  auto instance = skyr::url{"http://example.com:8080/"};

  auto result = instance.set_hostname("elpmaxe.com");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://elpmaxe.com:8080/", instance.href());
  EXPECT_EQ("elpmaxe.com:8080", instance.host());
  EXPECT_EQ("elpmaxe.com", instance.hostname());
}

TEST(url_setter_tests, test_hostname_file_set_non_empty_host) {
  auto instance = skyr::url{"file:///path/to/helicon/"};

  auto result = instance.set_hostname("example.com");
  ASSERT_TRUE(result);
  EXPECT_EQ("file://example.com/path/to/helicon/", instance.href());
  EXPECT_EQ("example.com", instance.hostname());
  EXPECT_EQ("/path/to/helicon/", instance.pathname());
}

TEST(url_setter_tests, test_hostname_file_set_empty_host) {
  auto instance = skyr::url{"file://example.com/path/to/helicon/"};

  auto result = instance.set_hostname("");
  ASSERT_TRUE(result);
  EXPECT_EQ("file:///path/to/helicon/", instance.href());
  EXPECT_EQ("", instance.hostname());
  EXPECT_EQ("/path/to/helicon/", instance.pathname());
}

TEST(url_setter_tests, test_port_no_port) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_port("8080");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com:8080/", instance.href());
}

TEST(url_setter_tests, test_port_existing_port) {
  auto instance = skyr::url{"http://example.com:80/"};

  auto result = instance.set_port("8080");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com:8080/", instance.href());
}

TEST(url_setter_tests, test_port_existing_port_no_port_1) {
  auto instance = skyr::url{"http://example.com:80/"};

  auto result = instance.set_port("");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/", instance.href());
}

TEST(url_setter_tests, DISABLED_test_port_invalid_port_1) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_port("Ceci n'est pas un port");
  ASSERT_FALSE(result);
  EXPECT_EQ("http://example.com/", instance.href());
}

TEST(url_setter_tests, test_port_invalid_port_2) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_port("1234567890");
  ASSERT_FALSE(result);
  EXPECT_EQ("http://example.com/", instance.href());
}

TEST(url_setter_tests, test_port_existing_port_no_port_2) {
  auto instance = skyr::url{"http://example.com:/"};

  auto result = instance.set_port("");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/", instance.href());
}

//TEST(url_setter_tests, test_port_no_port_int) {
//  auto instance = skyr::url{"http://example.com/"};
//
//  auto result = instance.set_port(8080);
//  ASSERT_TRUE(result);
//  EXPECT_EQ("http://example.com:8080/", instance.href());
//}

TEST(url_setter_tests, test_pathname_1) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_pathname("/path/to/helicon/");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/path/to/helicon/", instance.href());
}

TEST(url_setter_tests, test_pathname_2) {
  auto instance = skyr::url{"http://example.com/path/to/helicon/"};

  auto result = instance.set_pathname("");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/", instance.href());
}

TEST(url_setter_tests, test_pathname_3) {
  auto instance = skyr::url{"file:///path/to/helicon/"};

  auto result = instance.set_pathname("");
  ASSERT_TRUE(result);
  EXPECT_EQ("file:///", instance.href());
}

TEST(url_setter_tests, test_search_1) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_search("?a=b&c=d");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/?a=b&c=d", instance.href());
}

TEST(url_setter_tests, test_search_2) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_search("a=b&c=d");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/?a=b&c=d", instance.href());
}

TEST(url_setter_tests, test_search_3) {
  auto instance = skyr::url{"http://example.com/#fragment"};

  auto result = instance.set_search("?a=b&c=d");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/?a=b&c=d#fragment", instance.href());
}

TEST(url_setter_tests, test_hash_1) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_hash("#fragment");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/#fragment", instance.href());
}

TEST(url_setter_tests, test_hash_2) {
  auto instance = skyr::url{"http://example.com/"};

  auto result = instance.set_hash("fragment");
  ASSERT_TRUE(result);
  EXPECT_EQ("http://example.com/#fragment", instance.href());
}
