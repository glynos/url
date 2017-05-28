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
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_with_single_kvp_in_initalizer_list) {
  network::uri::query_parameters parameters{{"a", "b"}};

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_with_two_kvps) {
  network::uri::query_parameters parameters{"a=b&c=d"};

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_with_two_kvps_in_initializer_list) {
  network::uri::query_parameters parameters{{"a", "b"}, {"c", "d"}};

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_with_two_kvps_using_semicolon_separator) {
  network::uri::query_parameters parameters{"a=b;c=d"};

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
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_append_one_kvp) {
  network::uri::query_parameters parameters{};
  parameters.append("a", "b");

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_append_two_kvps) {
  network::uri::query_parameters parameters{};
  parameters.append("a", "b");
  parameters.append("c", "d");

  auto it = parameters.begin();
  ASSERT_NE(it, parameters.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, parameters.end());
}

TEST(uri_query_parameters_test, query_append_one_kvp_to_string) {
  network::uri::query_parameters parameters{};
  parameters.append("a", "b");

  EXPECT_EQ("a=b", parameters.to_string());
}

TEST(uri_query_parameters_test, query_append_two_kvps_to_string) {
  network::uri::query_parameters parameters{};
  parameters.append("a", "b");
  parameters.append("c", "d");

  EXPECT_EQ("a=b&c=d", parameters.to_string());
}

TEST(uri_query_parameters_test, query_sort_test) {
  // https://url.spec.whatwg.org/#example-searchparams-sort
  network::uri::query_parameters parameters{"c=d&a=b"};
  parameters.sort();
  EXPECT_EQ("a=b&c=d", parameters.to_string());
}

TEST(uri_query_parameters_test, copy_test) {
  network::uri::query_parameters parameters{{"a", "b"}, {"c", "d"}};
  network::uri::query_parameters copy(parameters);

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(uri_query_parameters_test, copy_assignment_test) {
  network::uri::query_parameters parameters{{"a", "b"}, {"c", "d"}};
  network::uri::query_parameters copy;
  copy = parameters;

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(uri_query_parameters_test, move_test) {
  network::uri::query_parameters parameters{{"a", "b"}, {"c", "d"}};
  network::uri::query_parameters copy(std::move(parameters));

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}

TEST(uri_query_parameters_test, move_assignment_test) {
  network::uri::query_parameters parameters{{"a", "b"}, {"c", "d"}};
  network::uri::query_parameters copy;
  copy = std::move(parameters);

  auto it = copy.begin();
  ASSERT_NE(it, copy.end());
  EXPECT_EQ("a", it->first);
  EXPECT_EQ("b", it->second);
  ++it;
  EXPECT_EQ("c", it->first);
  EXPECT_EQ("d", it->second);
  ++it;
  EXPECT_EQ(it, copy.end());
}
