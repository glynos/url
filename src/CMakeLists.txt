# Copyright (c) Glyn Matthews 2012-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(Uri_SRCS
        url_schemes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_errors.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_errors.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parser_context.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parser_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_record.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_parse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_schemes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/url_search_parameters.cpp

        ${CMAKE_SOURCE_DIR}/include/skyr/optional.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/string_view.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/config.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url/details/decode.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url/details/encode.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url/details/translate.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url_state.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url_record.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url_parse.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url_serialize.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url_search_parameters.hpp
        ${CMAKE_SOURCE_DIR}/include/skyr/url_path_iterator.hpp)

add_library(skyr ${Uri_SRCS})
target_link_libraries(skyr)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  if (NOT Uri_DISABLE_LIBCXX)
    target_link_libraries(skyr "c++")
  endif()
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR
    ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
endif()

#propagate sources to parent scope for one-lib-build
set(Uri_SRCS ${Uri_SRCS} PARENT_SCOPE)

install(
  TARGETS skyr
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
