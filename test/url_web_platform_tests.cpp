// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <network/url.hpp>
#include "test_uri.hpp"
#include "string_utility.hpp"
#include "json.hpp"

// Tests using test data from W3C
// https://github.com/w3c/web-platform-tests/tree/master/url

using json = nlohmann::json;

namespace {
struct test_case {
  test_case(json object)
    : failure(false) {
    input = object["input"].get<std::string>();

    if (object.find("failure") != object.end()) {
      failure = object["failure"].get<bool>();
    }
  }

  std::string input;
  bool failure;
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
      if (test_case_data.failure) {
        test_data.emplace_back(test_case_data);
      }
    }
  }
  return test_data;
}

class test_parse_urls : public ::testing::TestWithParam<test_case> {};

INSTANTIATE_TEST_CASE_P(url_web_platform_tests, test_parse_urls,
                        testing::ValuesIn(load_test_data()));

TEST_P(test_parse_urls, url_web_platform_tests) {
  auto test_case_data = test_case{GetParam()};
  std::cout << test_case_data.input << std::endl;

  auto url = test::uri{test_case_data.input};

  if (test_case_data.failure) {
    EXPECT_THROW(network::url{test_case_data.input}, network:: uri_syntax_error);
  }
  else {
    // auto parsed = url.parse_uri();
    // if (!parsed) {
    //   std::cout << " |" << url.parsed_till() << "|" << std::endl;
    // }
    // EXPECT_TRUE(parsed);
  }
}
