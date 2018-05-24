// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <network/url.hpp>
#include <network/uri/detail/uri_parse.hpp>
// #include "test_uri.hpp"
#include "string_utility.hpp"
#include "json.hpp"
#include "../src/detail/algorithm.hpp"

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

std::vector<test_case> load_test_data() {
  std::ifstream fs{"urltestdata.json"};
  json tests;
  fs >> tests;

  std::vector<test_case> test_data;
  for (auto &&test_case_object : tests) {
    if (!test_case_object.is_string()) {
      auto test_case_data = test_case{test_case_object};
      test_data.emplace_back(test_case_data);
    }
  }
  return test_data;
}

class test_parse_urls : public ::testing::TestWithParam<test_case> {};

INSTANTIATE_TEST_CASE_P(url_web_platform_tests, test_parse_urls,
                        testing::ValuesIn(load_test_data()));

TEST_P(test_parse_urls, url_web_platform_tests) {
  auto test_case_data = test_case{GetParam()};

  if (test_case_data.is_absolute()) {
    if (test_case_data.failure) {
      EXPECT_THROW(network::url{test_case_data.input}, network::uri_syntax_error);
      std::cout << " >" << test_case_data.input << "<" << std::endl;
    }
    else {

//      {
//        ::network::detail::trim_front(test_case_data.input);
//        ::network::detail::trim_back(test_case_data.input);
//
//        auto it = std::remove_if(std::begin(test_case_data.input), std::end(test_case_data.input),
//                                 [](char c) -> bool { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; });
//        std::cout << test_case_data.input << std::endl;
//        test_case_data.input.erase(it);
//      }

      auto view = network::string_view(test_case_data.input);
      auto it = std::begin(view), last = std::end(view);
      auto result = network::detail::parse(it, last);

      if (!result.success) {
        std::cout << " >" << test_case_data.input << "<" << std::endl;
        std::cout << " |" << std::string(std::begin(view), it) << "|" << std::endl;
      }
      else {
        std::cout << " >" << test_case_data.input << "<" << std::endl;
        EXPECT_EQ(test_case_data.protocol, result.scheme);
        EXPECT_EQ(test_case_data.username, result.username);
        EXPECT_EQ(test_case_data.password, result.password);
        EXPECT_EQ(test_case_data.hostname, result.hostname);
        EXPECT_EQ(test_case_data.port, result.port);
//        EXPECT_EQ(test_case_data.pathname, result.path);
//        EXPECT_EQ(test_case_data.search, result.query);
//        EXPECT_EQ(test_case_data.hash, result.fragment);
//
//        std::cout << test_case_data.input << std::endl;
//        std::cout << "=================" << std::endl;
//        std::cout << result.scheme << std::endl;
//        std::cout << result.username << ":" << result.password << std::endl;
//        std::cout << result.hostname << std::endl;
//        std::cout << result.port << std::endl;
//        std::cout << result.path << std::endl;
//        std::cout << result.query << std::endl;
//        std::cout << result.fragment << std::endl;
//        std::cout << "=================" << std::endl;
      }
      // EXPECT_TRUE(result.success);
    }
  }
//  else {
//    auto base = network::url(test_case_data.base);
//    if (test_case_data.failure) {
//      //    std::cout << " >" << test_case_data.input << "<" << std::endl;
//      EXPECT_THROW(network::url(test_case_data.input, base), network::uri_syntax_error);
//    }
//    else {
//      EXPECT_NO_THROW(network::url(test_case_data.input, base));
//    }
//  }
}
