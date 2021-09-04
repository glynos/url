// Copyright 2019-21 Glyn Matthews
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <memory>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
import skyr.v3.url;

using namespace skyr::literals;

TEST_CASE("url_tests", "[url]") {
  SECTION("construct url from literal") {
    CHECK_NOTHROW("http://www.example.com/"_url);
  }

  SECTION("construct url from char16_t literal") {
    CHECK_NOTHROW(u"http://www.example.com/"_url);
  }

  SECTION("construct url from char32_t literal") {
    CHECK_NOTHROW(U"http://www.example.com/"_url);
  }
}
