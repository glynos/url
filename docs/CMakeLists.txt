# Copyright (c) Glyn Matthews 2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(Doxygen)

if (DOXYGEN_FOUND)
  # API documentation
  configure_file(
          ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
          ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  file(COPY _build DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY _static DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY _templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY index.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY url.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY url_search_parameters.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY optional.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY expected.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY percent_encode.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY domain.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY filesystem.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY unicode.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  configure_file(
          ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in
          ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

  configure_file(
          ${CMAKE_CURRENT_SOURCE_DIR}/generate_docs.sh.in
          ${CMAKE_CURRENT_BINARY_DIR}/generate_docs.sh @ONLY)

  add_custom_target(doc
          ${CMAKE_CURRENT_BINARY_DIR}/generate_docs.sh
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
          COMMENT "Generating API documentation with Doxygen + Sphinx" VERBATIM)
endif()
