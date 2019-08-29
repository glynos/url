// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../src/url/idna_table.hpp"

TEST_CASE("IDNA character values", "[idna]") {
  using param = std::pair<char32_t, skyr::idna_status>;

  auto code_point = GENERATE(
          param{0x0000, skyr::idna_status::disallowed_std3_valid},
          param{0x002d, skyr::idna_status::valid},
          param{0x10fffd, skyr::idna_status::disallowed},
          param{0x10ffff, skyr::idna_status::disallowed});
  
  SECTION("code_point_set") {
    REQUIRE(code_point.second == skyr::map_idna_status(code_point.first));
  }
}
