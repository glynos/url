// Copyright (c) Glyn Matthews 2016-18.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

TEST(optional_tests, empty_optional) {
  skyr::optional<int> opt;
  ASSERT_FALSE(opt);
}

TEST(optional_tests, empty_optional_constructed_with_nullopt) {
  skyr::optional<int> opt{skyr::nullopt};
  ASSERT_FALSE(opt);
}

TEST(optional_tests, empty_optional_string) {
  skyr::optional<std::string> opt{};
  ASSERT_FALSE(opt);
}

TEST(optional_tests, empty_optional_string_with_nullopt) {
  skyr::optional<std::string> opt{skyr::nullopt};
  ASSERT_FALSE(opt);
}

TEST(optional_tests, value_constructor) {
  skyr::optional<int> opt{42};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_tests, value_constructor_string) {
  skyr::optional<std::string> opt{"banana"};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_tests, rvalue_ref_constructor) {
  int value = 42;
  skyr::optional<int> opt{std::move(value)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_tests, rvalue_ref_constructor_string) {
  std::string value = "banana";
  skyr::optional<std::string> opt{std::move(value)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_tests, nullopt_copy_constructor) {
  skyr::optional<int> other{skyr::nullopt};
  skyr::optional<int> opt{other};
  ASSERT_FALSE(opt);
}

TEST(optional_tests, nullopt_move_constructor) {
  skyr::optional<int> other{skyr::nullopt};
  skyr::optional<int> opt{std::move(other)};
  ASSERT_FALSE(opt);
}

TEST(optional_tests, value_copy_constructor) {
  skyr::optional<int> other{42};
  skyr::optional<int> opt{other};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_tests, value_move_constructor) {
  skyr::optional<int> other{42};
  skyr::optional<int> opt{std::move(other)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_tests, value_copy_constructor_string) {
  skyr::optional<std::string> other{"banana"};
  skyr::optional<std::string> opt{other};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_tests, value_move_constructor_string) {
  skyr::optional<std::string> other{"banana"};
  skyr::optional<std::string> opt{std::move(other)};
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_tests, nullopt_assignment) {
  skyr::optional<int> opt(42);
  opt = skyr::nullopt;
  ASSERT_FALSE(opt);
}

TEST(optional_tests, nullopt_assignment_string) {
  skyr::optional<std::string> opt("banana");
  opt = skyr::nullopt;
  ASSERT_FALSE(opt);
}

TEST(optional_tests, value_copy_assigment) {
  skyr::optional<int> opt{};
  skyr::optional<int> other{42};
  opt = other;
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_tests, value_move_assignment) {
  skyr::optional<int> opt{};
  skyr::optional<int> other{42};
  opt = std::move(other);
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, 42);
}

TEST(optional_tests, value_copy_assignment_string) {
  skyr::optional<std::string> opt{};
  skyr::optional<std::string> other{"banana"};
  opt = other;
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_tests, value_move_assignment_string) {
  skyr::optional<std::string> opt{};
  skyr::optional<std::string> other{"banana"};
  opt = std::move(other);
  ASSERT_TRUE(opt);
  ASSERT_EQ(*opt, "banana");
}

TEST(optional_tests, value_or_reference) {
  skyr::optional<std::string> opt;
  auto result = opt.value_or("other");
  ASSERT_EQ("other", result);
}

TEST(optional_tests, value_or_reference_with_value) {
  skyr::optional<std::string> opt("this");
  auto result = opt.value_or("other");
  ASSERT_EQ("this", result);
}

TEST(optional_tests, value_or_rvalue_reference) {
  std::string other("other");
  auto result = skyr::optional<std::string>().value_or(other);
  ASSERT_EQ("other", result);
}

TEST(optional_tests, value_or_rvalue_reference_with_value) {
  std::string other("other");
  auto result = skyr::optional<std::string>("this").value_or(other);
  ASSERT_EQ("this", result);
}

TEST(optional_tests, assign_nullopt_to_nullopt) {
  skyr::optional<std::string> opt;
  opt = skyr::nullopt;
  ASSERT_FALSE(opt);
}
