// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <skyr/url.hpp>
#include <skyr/filesystem.hpp>

TEST(filesystem_path_tests, empty_path) {
  auto instance = skyr::url{};
  ASSERT_FALSE(skyr::filesystem::to_path(instance));
}

TEST(filesystem_path_tests, file_path) {
  auto instance = skyr::url{"file:///path/to/file.txt"};
  auto path = skyr::filesystem::to_path(instance);
  ASSERT_TRUE(path);
  EXPECT_EQ(path.value().generic_string(), "/path/to/file.txt");
}

TEST(filesystem_path_tests, http_path) {
  auto instance = skyr::url{"http://www.example.com/path/to/file.txt"};
  auto path = skyr::filesystem::to_path(instance);
  ASSERT_TRUE(path);
  EXPECT_EQ(path.value().generic_string(), "/path/to/file.txt");
}
