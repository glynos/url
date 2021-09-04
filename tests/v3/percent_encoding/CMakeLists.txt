# Copyright (c) Glyn Matthews 2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (file_name
         percent_decoding_tests.cpp
         percent_encoding_tests.cpp
        )
    skyr_create_v3_test(${file_name} ${PROJECT_BINARY_DIR}/tests/percent_encoding test_name)

    add_dependencies(${test_name} skyr-percent_encoding-v3)
    target_link_libraries(
            ${test_name}
            PRIVATE
            skyr-percent_encoding-v3
    )
endforeach ()
