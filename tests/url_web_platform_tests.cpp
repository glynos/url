// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <skyr/url.hpp>
#include "json.hpp"

// Tests using test data from W3C
// https://github.com/w3c/web-platform-tests/tree/master/url

using json = nlohmann::json;

namespace {
struct test_case {
  test_case(json object)
    : failure(false) {
    input = object["input"].get<std::string>();
    base = object["base"].get<std::string>();

    if (object.find("failure") != object.end()) {
      failure = object["failure"].get<bool>();
    }

    if (!failure) {
      href = object["href"].get<std::string>();
      // origin = object["origin"].get<std::string>();
      protocol = object["protocol"].get<std::string>();
      username = object["username"].get<std::string>();
      password = object["password"].get<std::string>();
      host = object["host"].get<std::string>();
      hostname = object["hostname"].get<std::string>();
      port = object["port"].get<std::string>();
      pathname = object["pathname"].get<std::string>();
      search = object["search"].get<std::string>();
      hash = object["hash"].get<std::string>();
    }
  }

  std::string input;
  std::string base;
  bool failure;
  std::string href;
  std::string origin;
  std::string protocol;
  std::string username;
  std::string password;
  std::string host;
  std::string hostname;
  std::string port;
  std::string pathname;
  std::string search;
  std::string hash;
};
} // namespace

std::vector<test_case> load_test_data(bool failing) {
  std::ifstream fs{"urltestdata.json"};
  json tests;
  fs >> tests;

  std::vector<test_case> test_data;
  for (auto &&test_case_object : tests) {
    if (!test_case_object.is_string()) {
      auto test_case_data = test_case{test_case_object};
      if (failing == test_case_data.failure) {
        test_data.emplace_back(test_case_data);
      }
    }
  }
  return test_data;
}

class test_parse_urls_using_base_urls : public ::testing::TestWithParam<test_case> {};

INSTANTIATE_TEST_CASE_P(url_web_platform_tests, test_parse_urls_using_base_urls,
                        testing::ValuesIn(load_test_data(false)));

TEST_P(test_parse_urls_using_base_urls, DISABLED_parse_using_constructor) {
  auto test_case_data = test_case{GetParam()};
  auto instance = skyr::url(test_case_data.input, skyr::url(test_case_data.base));
  EXPECT_EQ(test_case_data.protocol, instance.protocol())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.username, instance.username())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.password, instance.password())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.host, instance.host())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.hostname, instance.hostname())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.port, instance.port())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.pathname, instance.pathname())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.search, instance.search())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.hash, instance.hash())
            << "Input: [" << test_case_data.input << "]";
}

TEST_P(test_parse_urls_using_base_urls, parse_using_make) {
  auto test_case_data = test_case{GetParam()};
  auto instance = skyr::make_url(test_case_data.input, skyr::url(test_case_data.base));
  ASSERT_TRUE(instance);
  EXPECT_EQ(test_case_data.protocol, instance.value().protocol())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.username, instance.value().username())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.password, instance.value().password())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.host, instance.value().host())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.hostname, instance.value().hostname())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.port, instance.value().port())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.pathname, instance.value().pathname())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.search, instance.value().search())
            << "Input: [" << test_case_data.input << "]";
  EXPECT_EQ(test_case_data.hash, instance.value().hash())
            << "Input: [" << test_case_data.input << "]";
}

class test_parse_urls_using_base_urls_failing : public ::testing::TestWithParam<test_case> {};

INSTANTIATE_TEST_CASE_P(url_web_platform_tests, test_parse_urls_using_base_urls_failing,
                        testing::ValuesIn(load_test_data(true)));

TEST_P(test_parse_urls_using_base_urls_failing, DISABLED_parse_using_constructor) {
  auto test_case_data = test_case{GetParam()};
  auto base = skyr::url(test_case_data.base);
  ASSERT_THROW(skyr::url(test_case_data.input, base), skyr::url_parse_error)
                << "Input: [" << test_case_data.input << "]";
}

TEST_P(test_parse_urls_using_base_urls_failing, parse_using_make) {
  auto test_case_data = test_case{GetParam()};
  auto base = skyr::url(test_case_data.base);
  ASSERT_FALSE(skyr::make_url(test_case_data.input, base))
                << "Input: [" << test_case_data.input << "]";
}
