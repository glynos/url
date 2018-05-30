// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <skyr.hpp>


TEST(path_iterator_test, empty_string) {
  auto string = std::string("");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_TRUE(it == skyr::path_iterator());
}

TEST(path_iterator_test, empty_string_distance) {
  auto string = std::string("");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view), last = skyr::path_iterator();
  ASSERT_EQ(0, std::distance(it, last));
}

TEST(path_iterator_test, single_slash) {
  auto string = std::string("/");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
}

TEST(path_iterator_test, single_slash_and_increment) {
  auto string = std::string("/");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
  ++it;
  ASSERT_TRUE(it == skyr::path_iterator());
}

TEST(path_iterator_test, single_slash_distance) {
  auto string = std::string("/");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view), last = skyr::path_iterator();
  ASSERT_EQ(1, std::distance(it, last));
}

TEST(path_iterator_test, double_slash) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
}

TEST(path_iterator_test, double_slash_and_increment_once) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
  ++it;
  ASSERT_FALSE(it == skyr::path_iterator());
}

TEST(path_iterator_test, double_slash_and_increment_twice) {
  auto string = std::string("//");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
  ++it;
  ++it;
  ASSERT_TRUE(it == skyr::path_iterator());
}

TEST(path_iterator_test, single_backslash) {
  auto string = std::string("\\");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
}

TEST(path_iterator_test, single_backslash_and_increment) {
  auto string = std::string("\\");
  auto view = skyr::string_view(string);
  auto it = skyr::path_iterator(view);
  ASSERT_FALSE(it == skyr::path_iterator());
  ++it;
  ASSERT_TRUE(it == skyr::path_iterator());
}
