# Copyright (c) Glyn Matthews 2012-2018.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


cmake_minimum_required(VERSION 3.8)
project(Skyr)

option(Skyr_BUILD_TESTS "Build the URI tests." ON)
option(Skyr_BUILD_DOCS "Build the URI documentation." ON)
option(Skyr_FULL_WARNINGS "Build the library with all warnings turned on." ON)
option(Skyr_WARNINGS_AS_ERRORS "Treat warnings as errors." ON)
option(Skyr_USE_STATIC_CRT "Use static C Runtime library (/MT or MTd)." ON)
option(Skyr_USE_STD17 "Use the C++ 17 standard (GCC only)" OFF)
option(Skyr_RUN_WEB_PLATFORM_TESTS "Run web platform tests" OFF)

find_package(Threads REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE true)

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  INCLUDE(CheckCXXCompilerFlag)

  if(Skyr_USE_STD17)
    CHECK_CXX_COMPILER_FLAG(-std=c++17 HAVE_STD17)
    if (HAVE_STD17)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else()
      message(FATAL_ERROR "No C++ 17 support (Compiler does not define -std=c++17).")
    endif()
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  endif()

  if (Skyr_FULL_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  endif()

  if (Skyr_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()

  message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_CXX_LINK_FLAGS}")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

  if (NOT Skyr_DISABLE_LIBCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()

  if (Skyr_FULL_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  endif()

  if (Skyr_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()

  message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_CXX_LINK_FLAGS}")
endif()


if (MSVC)
  if(DEFINED MSVC_VERSION AND MSVC_VERSION LESS 1900)
    message(FATAL_ERROR "Requires VS 2015 or later")
  endif()

  if (Skyr_USE_STATIC_CRT)
    # Replace dynamic MSVCRT linker flags with static version.
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
  endif(Skyr_USE_STATIC_CRT)

  add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

include_directories(${Skyr_SOURCE_DIR}/src ${Skyr_SOURCE_DIR}/include)

add_subdirectory(src)

# Testing
if (Skyr_BUILD_TESTS)
  message(STATUS "Configuring tests")
  enable_testing()
  add_subdirectory(deps/googletest)
  add_subdirectory(test)
endif()

# Documentation
if (Skyr_BUILD_DOCS)
  message("Configuring documentation")
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM)
  endif()
endif()

install(DIRECTORY include DESTINATION ".")
