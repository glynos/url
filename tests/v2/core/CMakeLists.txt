# Copyright (c) Glyn Matthews 2012-2020.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (file_name
        parse_host_tests.cpp
        url_parse_tests.cpp
        parse_path_tests.cpp
        parse_query_tests.cpp
        url_serialize_tests.cpp
        )
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/core test_name v2)
endforeach ()
