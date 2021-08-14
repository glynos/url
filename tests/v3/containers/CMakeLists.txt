# Copyright (c) Glyn Matthews 2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

foreach (
        file_name
        static_vector_tests.cpp)
    skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/container test_name v3)
#    target_compile_options(${test_name} PRIVATE $<${clang}:-fmodules>)
    target_sources(${test_name} PRIVATE
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3/containers.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/v3.ixx
            ${PROJECT_SOURCE_DIR}/modules/skyr/skyr.ixx
    )
endforeach ()
