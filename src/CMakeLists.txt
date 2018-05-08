# Copyright (c) Glyn Matthews 2012-2017.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(Uri_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/uri_errors.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_parse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_parse_authority.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_advance_parts.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_normalize.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/uri_resolve.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/detail/url_schemes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ietf/uri.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ietf/uri_builder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/whatwg/url.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/whatwg/url_search_parameters.cpp

  ${CMAKE_SOURCE_DIR}/include/network/optional.hpp
  ${CMAKE_SOURCE_DIR}/include/network/string_view.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri.hpp
  ${CMAKE_SOURCE_DIR}/include/network/url.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/config.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/uri_errors.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/detail/decode.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/detail/encode.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/detail/translate.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/detail/uri_parts.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/ietf/uri.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/ietf/uri_builder.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/ietf/uri_io.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/whatwg/url.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/whatwg/url_io.hpp
  ${CMAKE_SOURCE_DIR}/include/network/uri/whatwg/url_search_parameters.hpp
  )

add_library(network-url ${Uri_SRCS})
target_link_libraries(network-url)
if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  if (NOT Uri_DISABLE_LIBCXX)
    target_link_libraries(network-url "c++")
  endif()
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR
    ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-declarations")
endif()

#propagate sources to parent scope for one-lib-build
set(Uri_SRCS ${Uri_SRCS} PARENT_SCOPE)

install(
  TARGETS network-url
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
