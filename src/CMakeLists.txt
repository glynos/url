# Copyright (c) Glyn Matthews 2012-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(Uri_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/url_errors.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/algorithm.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/grammar.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/url_parse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_advance_parts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_advance_parts.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/url_schemes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/url_schemes.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_percent_encode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/skyr/url.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/skyr/url_search_parameters.cpp

  ${CMAKE_SOURCE_DIR}/include/skyr/optional.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/string_view.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/config.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/url_errors.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/detail/decode.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/detail/encode.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/detail/translate.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/detail/uri_parts.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/url_parse.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/url.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/url_io.hpp
  ${CMAKE_SOURCE_DIR}/include/skyr/url/url_search_parameters.hpp
  )

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
