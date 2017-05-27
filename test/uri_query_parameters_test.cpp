// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri.hpp>
#include "string_utility.hpp"

TEST(uri_query_parameters_test, empty_query) {
  network::uri::query_parameters parameters{};
  EXPECT_EQ("", parameters.to_string());
  EXPECT_EQ(parameters.begin(), parameters.end());
}

TEST(uri_query_parameters_test, query_with_single_kvp) {
  network::uri::query_parameters parameters{"a=b"};

  EXPECT_EQ("a=b", parameters.to_string());
  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_with_two_kvps) {
  network::uri::query_parameters parameters{"a=b&c=d"};

  EXPECT_EQ("a=b&c=d", parameters.to_string());
  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  ASSERT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_with_two_kvps_using_semicolon_separator) {
  network::uri::query_parameters parameters{"a=b;c=d"};

  EXPECT_EQ("a=b;c=d", parameters.to_string());
  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  ASSERT_EQ(it, parameters.end());
}
