// Copyright 2020-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string_view>
#include <variant>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
import skyr.v3.core.host;

TEST_CASE("parse_host_tests", "url.parse_host") {
  using namespace std::string_view_literals;

  SECTION("is special") {
    SECTION("parse domain name") {
      auto host = skyr::parse_host("example.com"sv, false);
      REQUIRE(host);
      CHECK_NOTHROW(host.value().serialize());
    }

    SECTION("parse IPv4 address name") {
      auto host = skyr::parse_host("127.0.0.1"sv, false);
      REQUIRE(host);
      CHECK_NOTHROW(host.value().to_ipv4_address());
    }

    SECTION("parse IPv6 address name") {
      auto host = skyr::parse_host("[1080:0:0:0:8:800:200C:417A]"sv, false);
      REQUIRE(host);
      CHECK_NOTHROW(host.value().to_ipv6_address());
    }

    SECTION("parse invalid IPv4 address") {
      auto host = skyr::parse_host("127.0.0.266"sv, false);
      REQUIRE_FALSE(host);
    }

    SECTION("parse some garbage") {
      auto host = skyr::parse_host("x\t4\xdf!<>?"sv, false);
      REQUIRE_FALSE(host);
    }
  }

  SECTION("is not special") {
    SECTION("parse IPv4 address that is not special that is treated like an opaque host") {
      auto host = skyr::parse_host("127.0.0.1"sv, true);
      REQUIRE(host);
      CHECK_NOTHROW(host.value().serialize());
    }

    SECTION("parse invalid IPv4 address that is not special that is treated like an opaque host") {
      auto host = skyr::parse_host("127.0.0.266"sv, true);
      REQUIRE(host);
      CHECK_NOTHROW(host.value().serialize());
    }

    SECTION("parse IPv6 address name") {
      auto host = skyr::parse_host("[1080:0:0:0:8:800:200C:417A]"sv, true);
      REQUIRE(host);
      CHECK_NOTHROW(host.value().to_ipv6_address());
    }
  }
}
