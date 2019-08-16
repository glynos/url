// Copyright (c) Glyn Matthews 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include "../src/url/idna_table.hpp"

using code_point = std::pair<char32_t, skyr::idna_status>;

class code_point_tests : public ::testing::TestWithParam<code_point> {};

INSTANTIATE_TEST_SUITE_P(
    idna_table_tests,
    code_point_tests,
    ::testing::Values(
        code_point{0x0000, skyr::idna_status::disallowed_std3_valid},
        code_point{0x002d, skyr::idna_status::valid},
        code_point{0x10fffd, skyr::idna_status::disallowed},
        code_point{0x10ffff, skyr::idna_status::disallowed}
    ));

TEST_P(code_point_tests, code_point_set) {
  auto code_point = GetParam();
  ASSERT_EQ(code_point.second, skyr::map_idna_status(code_point.first));
}
