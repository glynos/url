# Copyright (c) Glyn Matthews 2012-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


include_directories( ${gtest_SOURCE_DIR}/include)

set(
  TESTS
  optional_tests
  encoding_tests
  url_tests
  url_web_platform_tests
  url_parse_tests
  url_search_parameters_tests
  )

foreach (test ${TESTS})
  add_executable(${test} ${test}.cpp)
  add_dependencies(${test} skyr gtest_main)
  target_link_libraries(${test} ${CMAKE_THREAD_LIBS_INIT} skyr gtest_main)
  if (OPENSSL_FOUND)
    target_link_libraries(${test} ${OPENSSL_LIBRARIES})
  endif()
  set_target_properties(${test}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Skyr_BINARY_DIR}/tests)
  add_test(${test}
    ${Skyr_BINARY_DIR}/tests/${test})
endforeach (test)

file(GLOB URI_LISTS *.txt *.json)
file(COPY ${URI_LISTS} DESTINATION ${Skyr_BINARY_DIR}/tests)
