# Copyright (c) Glyn Matthews 2019-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (
        file_name
        unicode_code_point_tests.cpp
        unicode_range_tests.cpp
        unicode_tests.cpp
        byte_conversion_tests.cpp
)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/unicode test_name v3)
    target_sources(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/concepts/url_concepts.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/unicode_errc.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/core.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/constants.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/code_points/code_point_u8.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/code_points/code_point_u16.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/code_point.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/traits/iterator_value.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/traits/range_iterator.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/traits/range_value.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/sentinel.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/transforms/transform_u8.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/transforms/transform_u16.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/transforms/transform_u32.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/views/view_unchecked_u8.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/views/view_u8.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/ranges/views/view_u16.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/unicode/u8_string.ixx
            )
endforeach ()
