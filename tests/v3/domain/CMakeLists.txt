# Copyright (c) Glyn Matthews 2019-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (
        file_name
        idna_table_tests.cpp)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/domain test_name v3)
    target_sources(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/domain/idna_tables.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/domain/idna_status.ixx
            )
endforeach ()
