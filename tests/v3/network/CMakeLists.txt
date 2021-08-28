# Copyright (c) Glyn Matthews 2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (file_name
        ipv4_address_tests.cpp
        ipv6_address_tests.cpp
        )
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/network test_name v3)
    target_sources(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/platform/endianness.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/containers/static_vector.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/network/ipv4_address_errc.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/network/ipv4.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/network/ipv6_address_errc.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/network/ipv6.ixx
    )
endforeach ()
