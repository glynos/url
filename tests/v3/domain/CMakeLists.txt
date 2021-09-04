# Copyright (c) Glyn Matthews 2019-21.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (
        file_name
        idna_table_tests.cpp
        punycode_tests.cpp
        domain_tests.cpp
)
    skyr_create_v3_test(${file_name} ${PROJECT_BINARY_DIR}/tests/domain test_name)

    add_dependencies(${test_name} skyr-domain-v3)
    target_link_libraries(
            ${test_name}
            PRIVATE
            skyr-domain-v3
    )
endforeach ()
