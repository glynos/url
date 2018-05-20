# Copyright (c) Glyn Matthews 2012-2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


include_directories( ${gtest_SOURCE_DIR}/include)

set(
  TESTS
# uri_parse_test
# uri_parse_scheme_test
# uri_parse_path_test
# uri_test
# uri_encoding_test
# uri_normalization_test
# uri_comparison_test
# uri_reference_test
# uri_resolve_test
# uri_builder_test
# uri_stream_test
# url_search_parameters_test
  url_test
  url_web_platform_tests
  optional_test
  )

if (Uri_RUN_WEB_PLATFORM_TESTS)
    list(APPEND TESTS url_web_platform_tests)
endif()

foreach (test ${TESTS})
  add_executable(${test} ${test}.cpp)
  add_dependencies(${test} network-url gtest_main)
  target_link_libraries(${test} ${CMAKE_THREAD_LIBS_INIT} network-url gtest_main)
  if (OPENSSL_FOUND)
    target_link_libraries(${test} ${OPENSSL_LIBRARIES})
  endif()
  set_target_properties(${test}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Uri_BINARY_DIR}/tests)
  add_test(${test}
    ${Uri_BINARY_DIR}/tests/${test})
endforeach (test)

file(GLOB URI_LISTS *.txt *.json)
file(COPY ${URI_LISTS} DESTINATION ${Uri_BINARY_DIR}/tests)
