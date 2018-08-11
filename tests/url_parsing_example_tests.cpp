// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <gtest/gtest.h>
#include <skyr/url_parse.hpp>
#include <skyr/url_serialize.hpp>

/// https://url.spec.whatwg.org/#example-url-parsing

TEST(url_parsing_example_tests, url_serialize_1) {
  auto input = std::string("https:example.org");
  auto instance = skyr::parse(input);
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.org/", output);
}

TEST(url_parsing_example_tests, url_serialize_2) {
  auto instance = skyr::parse("https://////example.com///");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.com///", output);
}

TEST(url_parsing_example_tests, url_serialize_3) {
  auto instance = skyr::parse("https://example.com/././foo");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.com/foo", output);
}

TEST(url_parsing_example_tests, url_serialize_4) {
  auto base = skyr::parse("https://example.com/");
  ASSERT_TRUE(base);
  auto instance = skyr::parse("hello:world", base.value());
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("hello:world", output);
}

TEST(url_parsing_example_tests, url_serialize_5) {
  auto base = skyr::parse("https://example.com/");
  ASSERT_TRUE(base);
  auto instance = skyr::parse("https:example.org", base.value());
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.com/example.org", output);
}

TEST(url_parsing_example_tests, url_serialize_6) {
  auto base = skyr::parse("https://example.com/");
  ASSERT_TRUE(base);
  auto instance = skyr::parse("\\example\\..\\demo/.\\", base.value());
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.com/demo/", output);
}

TEST(url_parsing_example_tests, url_serialize_7) {
  auto base = skyr::parse("https://example.com/demo");
  ASSERT_TRUE(base);
  auto instance = skyr::parse("example", base.value());
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.com/example", output);
}

TEST(url_parsing_example_tests, url_serialize_8) {
  auto instance = skyr::parse("file:///C|/demo");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("file:///C:/demo", output);
}

TEST(url_parsing_example_tests, url_serialize_9) {
  auto base = skyr::parse("file:///C:/demo");
  ASSERT_TRUE(base);
  auto instance = skyr::parse("..", base.value());
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("file:///C:/", output);
}

TEST(url_parsing_example_tests, url_serialize_10) {
  auto instance = skyr::parse("file://loc%61lhost/");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("file:///", output);
}

TEST(url_parsing_example_tests, url_serialize_11) {
  auto instance = skyr::parse("https://user:password@example.org/");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://user:password@example.org/", output);
}

TEST(url_parsing_example_tests, url_serialize_12) {
  auto instance = skyr::parse("https://example.org/foo bar");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.org/foo%20bar", output);
}

TEST(url_parsing_example_tests, url_serialize_13) {
  auto instance = skyr::parse("https://EXAMPLE.com/../x");
  ASSERT_TRUE(instance);
  auto output = skyr::serialize(instance.value());
  EXPECT_EQ("https://example.com/x", output);
}

TEST(url_parsing_example_tests, url_serialize_14) {
  auto instance = skyr::parse("https://ex ample.org/");
  ASSERT_FALSE(instance);
}

TEST(url_parsing_example_tests, url_serialize_15) {
  auto instance = skyr::parse("example");
  ASSERT_FALSE(instance);
}

TEST(url_parsing_example_tests, url_serialize_16) {
  auto instance = skyr::parse("https://example.com:demo");
  ASSERT_FALSE(instance);
}

TEST(url_parsing_example_tests, url_serialize_17) {
  auto instance = skyr::parse("http://[www.example.com]/");
  ASSERT_FALSE(instance);
}
