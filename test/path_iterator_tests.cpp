// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <skyr/url_path_iterator.hpp>


TEST(path_iterator_tests, empty_string) {
  auto string = std::string("");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_TRUE(it == last);
}

TEST(path_iterator_tests, empty_string_distance) {
  auto string = std::string("");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_EQ(0, std::distance(it, last));
}

TEST(path_iterator_tests, single_slash) {
  auto string = std::string("/");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
}

TEST(path_iterator_tests, single_slash_and_increment) {
  auto string = std::string("/");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
  ++it;
  ASSERT_TRUE(it == last);
}

TEST(path_iterator_tests, single_slash_distance) {
  auto string = std::string("/");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_EQ(2, std::distance(it, last));
}

TEST(path_iterator_tests, double_slash) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
}

TEST(path_iterator_tests, double_slash_and_increment_once) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
  ++it;
  ASSERT_FALSE(it == last);
}

TEST(path_iterator_tests, double_slash_and_increment_twice) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
  ++it;
  ++it;
  ++it;
  ASSERT_TRUE(it == last);
}

TEST(path_iterator_tests, double_slash_distance) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_EQ(3, std::distance(it, last));
}

TEST(path_iterator_tests, single_backslash) {
  auto string = std::string("\\");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
}

TEST(path_iterator_tests, single_backslash_and_increment) {
  auto string = std::string("\\");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_FALSE(it == last);
  ++it;
  ASSERT_FALSE(it == last);
  ++it;
  ASSERT_TRUE(it == last);
}

TEST(path_iterator_Test, slash_foo_slash) {
  auto string = std::string("/foo/");
  auto view = skyr::string_view(string);
  auto it = skyr::url_path_iterator(view), last = skyr::url_path_iterator();
  ASSERT_EQ(3, std::distance(it, last));
}