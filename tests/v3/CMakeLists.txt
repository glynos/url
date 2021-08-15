# Copyright (c) Glyn Matthews 2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)

add_subdirectory(containers)
add_subdirectory(network)
add_subdirectory(percent_encoding)
