// Copyright 2020-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
import skyr.v3.core.parse_path;

TEST_CASE("path_parsing_example_tests", "[parse]") {
  SECTION("url_path_1") {
    auto path = skyr::parse_path("///");
    REQUIRE(path);
    CHECK(3 == path.value().size());
  }

  SECTION("url_path_2") {
    auto path = skyr::parse_path("/a/b/./c");
    REQUIRE(path);
    CHECK(3 == path.value().size());
    CHECK("a" == path.value()[0]);
    CHECK("b" == path.value()[1]);
    CHECK("c" == path.value()[2]);
  }

  SECTION("url_path_3") {
    auto path = skyr::parse_path("/a/b/../c");
    REQUIRE(path);
    CHECK(2 == path.value().size());
    CHECK("a" == path.value()[0]);
    CHECK("c" == path.value()[1]);
  }
}
