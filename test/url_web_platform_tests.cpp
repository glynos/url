// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <skyr.hpp>
#include "json.hpp"
#include "string_utility.hpp"

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

  bool is_absolute() const {
    return base == "about:blank";
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

std::vector<test_case> load_absolute_test_data() {
  std::ifstream fs{"urltestdata.json"};
  json tests;
  fs >> tests;

  std::vector<test_case> test_data;
  for (auto &&test_case_object : tests) {
    if (!test_case_object.is_string()) {
      auto test_case_data = test_case{test_case_object};
      if (test_case_data.is_absolute()) {
        test_data.emplace_back(test_case_data);
      }
    }
  }
  return test_data;
}

std::vector<test_case> load_test_data_using_base_urls() {
  std::ifstream fs{"urltestdata.json"};
  json tests;
  fs >> tests;

  std::vector<test_case> test_data;
  for (auto &&test_case_object : tests) {
    if (!test_case_object.is_string()) {
      auto test_case_data = test_case{test_case_object};
      if (!test_case_data.is_absolute()) {
        test_data.emplace_back(test_case_data);
      }
    }
  }
  return test_data;
}

class test_parse_urls_absolute : public ::testing::TestWithParam<test_case> {};

INSTANTIATE_TEST_CASE_P(url_web_platform_tests, test_parse_urls_absolute,
                        testing::ValuesIn(load_absolute_test_data()));

TEST_P(test_parse_urls_absolute, parse) {
  auto test_case_data = test_case{GetParam()};

  std::cout << " >" << test_case_data.input << "<" << std::endl;

  auto result = skyr::parse(test_case_data.input);

  if (test_case_data.failure) {
    if (result) {
      std::cout << " >" << test_case_data.input << "<" << std::endl;
    }

    EXPECT_FALSE(result);
  }
  else {
    if (!result) {
      std::cout << " >" << test_case_data.input << "<" << std::endl;
      return;
    }

    EXPECT_TRUE(result);
    EXPECT_EQ(test_case_data.protocol, result.scheme + ":");
    EXPECT_EQ(test_case_data.username, result.username);
    EXPECT_EQ(test_case_data.password, result.password);
    EXPECT_EQ(test_case_data.hostname.empty(), (!result.host || result.host.value().empty()));
    if (result.host) {
      EXPECT_EQ(test_case_data.hostname, result.host.value());
    }

    auto view = skyr::string_view(test_case_data.pathname);
    std::cout << test_case_data.pathname << std::endl;
    std::copy(
        begin(result.path), end(result.path),
        std::ostream_iterator<std::string>(std::cout, "/"));
    std::cout << std::endl;

    ASSERT_EQ(std::distance(skyr::path_iterator(view), skyr::path_iterator()), result.path.size());
    EXPECT_TRUE(std::equal(
        skyr::path_iterator(view), skyr::path_iterator(),
        std::begin(result.path),
        [](skyr::string_view lhs, const std::string &rhs) -> bool {
          return lhs.compare(rhs.c_str()) == 0;
        }));

    EXPECT_EQ(test_case_data.port.empty(), !result.port);
    if (result.port) {
      EXPECT_EQ(test_case_data.port, std::to_string(result.port.value()));
    }
    EXPECT_EQ(test_case_data.search.empty(), !result.query);
    if (result.query) {
      auto query = result.query.value().empty()? "" : "?" + result.query.value();
      EXPECT_EQ(test_case_data.search, query);
    }
    EXPECT_EQ(test_case_data.hash.empty(), (!result.fragment || result.fragment.value().empty()));
    if (result.fragment) {
      auto fragment = result.fragment.value().empty()? "" : "#" + result.fragment.value();
      EXPECT_EQ(test_case_data.hash, fragment);
    }
  }
}


class test_parse_urls_using_base_urls : public ::testing::TestWithParam<test_case> {};

INSTANTIATE_TEST_CASE_P(url_web_platform_tests, test_parse_urls_using_base_urls,
                        testing::ValuesIn(load_test_data_using_base_urls()));

TEST_P(test_parse_urls_using_base_urls, parse) {
  auto test_case_data = test_case{GetParam()};

  std::cout << " >" << test_case_data.input << "<" << std::endl;

//  if (test_case_data.input[0] == '/') {
//    FAIL();
//  }

  auto base = skyr::parse(test_case_data.base);
  ASSERT_TRUE(base);
  auto result = skyr::parse(test_case_data.input, base);

  if (test_case_data.failure) {
    if (result) {
      std::cout << " >" << test_case_data.input << "<" << std::endl;
    }

    EXPECT_FALSE(result);
  }
  else {
    if (!result) {
      std::cout << " >" << test_case_data.input << "<" << std::endl;
      return;
    }

    EXPECT_TRUE(result);
    EXPECT_EQ(test_case_data.protocol, result.scheme + ":");
    EXPECT_EQ(test_case_data.username, result.username);
    EXPECT_EQ(test_case_data.password, result.password);
    EXPECT_EQ(test_case_data.hostname.empty(), (!result.host || result.host.value().empty()));
    if (result.host) {
      EXPECT_EQ(test_case_data.hostname, result.host.value());
    }
//    EXPECT_EQ(test_case_data.pathname, join(result.path));
    EXPECT_EQ(test_case_data.port.empty(), !result.port);
    if (result.port) {
      EXPECT_EQ(test_case_data.port, std::to_string(result.port.value()));
    }
    EXPECT_EQ(test_case_data.search.empty(), !result.query);
    if (result.query) {
      auto query = result.query.value().empty()? "" : "?" + result.query.value();
      EXPECT_EQ(test_case_data.search, query);
    }
    EXPECT_EQ(test_case_data.hash.empty(), (!result.fragment || result.fragment.value().empty()));
    if (result.fragment) {
      auto fragment = result.fragment.value().empty()? "" : "#" + result.fragment.value();
      EXPECT_EQ(test_case_data.hash, fragment);
    }
  }
}
