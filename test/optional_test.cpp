// Copyright (c) Glyn Matthews 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <network/optional.hpp>
#include <network/string_view.hpp>

TEST(optional_test, empty_optional) {
  whatwg::optional<int> opt;
  ASSERT_FALSE(opt);
}

TEST(optional_test, empty_optional_constructed_with_nullopt) {
  whatwg::optional<int> opt{whatwg::nullopt};
  ASSERT_FALSE(opt);
}

TEST(optional_test, empty_optional_string) {
  whatwg::optional<std::string> opt{};
  ASSERT_FALSE(opt);
}

TEST(optional_test, empty_optional_string_with_nullopt) {
  whatwg::optional<std::string> opt{whatwg::nullopt};
  ASSERT_FALSE(opt);
}

TEST(optional_test, value_constructor) {
  whatwg::optional<int> opt{42};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_test, value_constructor_string) {
  whatwg::optional<std::string> opt{"banana"};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_test, rvalue_ref_constructor) {
  int value = 42;
  whatwg::optional<int> opt{std::move(value)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_test, rvalue_ref_constructor_string) {
  std::string value = "banana";
  whatwg::optional<std::string> opt{std::move(value)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_test, nullopt_copy_constructor) {
  whatwg::optional<int> other{whatwg::nullopt};
  whatwg::optional<int> opt{other};
  ASSERT_FALSE(opt);
}

TEST(optional_test, nullopt_move_constructor) {
  whatwg::optional<int> other{whatwg::nullopt};
  whatwg::optional<int> opt{std::move(other)};
  ASSERT_FALSE(opt);
}

TEST(optional_test, value_copy_constructor) {
  whatwg::optional<int> other{42};
  whatwg::optional<int> opt{other};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_test, value_move_constructor) {
  whatwg::optional<int> other{42};
  whatwg::optional<int> opt{std::move(other)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_test, value_copy_constructor_string) {
  whatwg::optional<std::string> other{"banana"};
  whatwg::optional<std::string> opt{other};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_test, value_move_constructor_string) {
  whatwg::optional<std::string> other{"banana"};
  whatwg::optional<std::string> opt{std::move(other)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_test, nullopt_assignment) {
  whatwg::optional<int> opt(42);
  opt = whatwg::nullopt;
  ASSERT_FALSE(opt);
}

TEST(optional_test, nullopt_assignment_string) {
  whatwg::optional<std::string> opt("banana");
  opt = whatwg::nullopt;
  ASSERT_FALSE(opt);
}

TEST(optional_test, value_copy_assigment) {
  whatwg::optional<int> opt{};
  whatwg::optional<int> other{42};
  opt = other;
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_test, value_move_assignment) {
  whatwg::optional<int> opt{};
  whatwg::optional<int> other{42};
  opt = std::move(other);
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_test, value_copy_assignment_string) {
  whatwg::optional<std::string> opt{};
  whatwg::optional<std::string> other{"banana"};
  opt = other;
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_test, value_move_assignment_string) {
  whatwg::optional<std::string> opt{};
  whatwg::optional<std::string> other{"banana"};
  opt = std::move(other);
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_test, value_or_reference) {
  whatwg::optional<std::string> opt;
  auto result = opt.value_or("other");
  ASSERT_EQ("other", result);
}

TEST(optional_test, value_or_reference_with_value) {
  whatwg::optional<std::string> opt("this");
  auto result = opt.value_or("other");
  ASSERT_EQ("this", result);
}

TEST(optional_test, value_or_rvalue_reference) {
  std::string other("other");
  auto result = whatwg::optional<std::string>().value_or(other);
  ASSERT_EQ("other", result);
}

TEST(optional_test, value_or_rvalue_reference_with_value) {
  std::string other("other");
  auto result = whatwg::optional<std::string>("this").value_or(other);
  ASSERT_EQ("this", result);
}

TEST(optional_test, assign_nullopt_to_nullopt) {
  whatwg::optional<std::string> opt;
  opt = whatwg::nullopt;
  ASSERT_FALSE(opt);
}
