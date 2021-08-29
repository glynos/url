# Copyright (c) Glyn Matthews 2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# foreach (file_name
#         percent_decoding_tests.cpp
#         percent_encoding_tests.cpp
#         )
#     skyr_create_test(${file_name} ${PROJECT_BINARY_DIR}/tests/percent_encoding test_name v3)
#     target_sources(${test_name} PRIVATE
#         ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/errors.ixx
#         ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/sentinel.ixx
#         ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/percent_encode.ixx
#         ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/percent_decode.ixx
#     )
# endforeach ()

skyr_create_test(percent_decoding_tests.cpp ${PROJECT_BINARY_DIR}/tests/percent_encoding test_name v3)
target_sources(${test_name} PRIVATE
        ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/percent_encode_errc.ixx
        ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/sentinel.ixx
        ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/percent_decode.ixx
)

skyr_create_test(percent_encoding_tests.cpp ${PROJECT_BINARY_DIR}/tests/percent_encoding test_name v3)
target_sources(${test_name} PRIVATE
        ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/percent_encode_errc.ixx
        ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/sentinel.ixx
        ${PROJECT_SOURCE_DIR}/modules/skyr/v3/percent_encoding/percent_encode.ixx
)
