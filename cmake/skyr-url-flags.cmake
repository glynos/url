# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in
# subdirectories)

# Compilation flags
include(CheckCXXCompilerFlag)
macro(sky_append_flag testname flag)
  # As -Wno-* flags do not lead to build failure when there are no other
  # diagnostics, we check positive option to determine their applicability.
  # Of course, we set the original flag that is requested in the parameters.
  string(REGEX REPLACE "^-Wno-" "-W" alt ${flag})
  check_cxx_compiler_flag(${alt} ${testname})
  if (${testname})
    add_compile_options(${flag})
  endif()
endmacro()

function(cxx_standard_normalize cxx_standard return_value)
  if("x${cxx_standard}" STREQUAL "x1y")
    set( ${return_value} "14" PARENT_SCOPE )
  elseif("x${cxx_standard}" STREQUAL "x1z")
    set( ${return_value} "17" PARENT_SCOPE )
  elseif("x${cxx_standard}" STREQUAL "xlatest" OR "x${cxx_standard}" STREQUAL "x2a")
    set( ${return_value} "20" PARENT_SCOPE )
  else()
    set( ${return_value} "${cxx_standard}" PARENT_SCOPE )
  endif()
endfunction()

function(cxx_standard_denormalize cxx_standard return_value)
  if("x${cxx_standard}" STREQUAL "x17")
    if (SKY_CXX_COMPILER_CLANGCL OR SKY_CXX_COMPILER_MSVC)
      set( ${return_value} 17 PARENT_SCOPE )
    else()
      set( ${return_value} 1z PARENT_SCOPE )
    endif()
  elseif("x${cxx_standard}" STREQUAL "x20")
    if (SKY_CXX_COMPILER_CLANGCL OR SKY_CXX_COMPILER_MSVC)
      set( ${return_value} latest PARENT_SCOPE )
    else()
      set( ${return_value} 2a PARENT_SCOPE )
    endif()
  else()
    set( ${return_value} ${cxx_standard} PARENT_SCOPE )
  endif()
endfunction()

if(CMAKE_CXX_STANDARD)
  if(NOT "x${SKY_CXX_STD}" STREQUAL "xdefault")
    # Normalize RANGES_CXX_STD
    cxx_standard_normalize( ${SKY_CXX_STD} sky_cxx_std )
    if(NOT "x${sky_cxx_std}" STREQUAL "x${CMAKE_CXX_STANDARD}")
      message(FATAL_ERROR "[skyr-url]: Cannot specify both CMAKE_CXX_STANDARD and SKY_CXX_STD, or they must match.")
    endif()
  else()
    cxx_standard_denormalize(${CMAKE_CXX_STANDARD} SKY_CXX_STD)
  endif()
elseif("x${SKY_CXX_STD}" STREQUAL "xdefault")
  if (SKY_CXX_COMPILER_CLANGCL OR SKY_CXX_COMPILER_MSVC)
    set(SKY_CXX_STD 17)
  else()
    set(SKY_CXX_STD 14)
  endif()
endif()

# All compilation flags
# Language flag: version of the C++ standard to use
message(STATUS "[skyr-url]: C++ std=${SKY_CXX_STD}")
if (SKY_CXX_COMPILER_CLANGCL OR SKY_CXX_COMPILER_MSVC)
  sky_append_flag(SKY_HAS_CXXSTDCOLON "/std:c++${SKY_CXX_STD}")
  set(SKY_STD_FLAG "/std:c++${SKY_CXX_STD}")
  if (SKY_CXX_COMPILER_CLANGCL)
    # The MSVC STL before VS 2019v16.6 with Clang 10 requires -fms-compatibility in C++17 mode, and
    # doesn't support C++20 mode at all. Let's drop this flag until AppVeyor updates to VS2016v16.6.
    # sky_append_flag(SKY_HAS_FNO_MS_COMPATIBIILITY "-fno-ms-compatibility")
    sky_append_flag(SKY_HAS_FNO_DELAYED_TEMPLATE_PARSING "-fno-delayed-template-parsing")
  endif()
  # Enable "normal" warnings and make them errors:
  #sky_append_flag(SKY_HAS_W3 /W3)
  #sky_append_flag(SKY_HAS_WX /WX)
else()
  sky_append_flag(SKY_HAS_CXXSTD "-std=c++${SKY_CXX_STD}")
  set(SKY_STD_FLAG "-std=c++${SKY_CXX_STD}")
  # Enable "normal" warnings and make them errors:
  #sky_append_flag(SKY_HAS_WALL -Wall)
  #sky_append_flag(SKY_HAS_WEXTRA -Wextra)
  if (SKY_ENABLE_WERROR)
    sky_append_flag(SKY_HAS_WERROR -Werror)
  endif()
endif()

if (SKY_ENV_LINUX AND SKY_CXX_COMPILER_CLANG)
  # On linux libc++ re-exports the system math headers. The ones from libstdc++
  # use the GCC __extern_always_inline intrinsic which is not supported by clang
  # versions 3.6, 3.7, 3.8, 3.9, 4.0, and current trunk 5.0 (as of 2017.04.13).
  #
  # This works around it by replacing __extern_always_inline with inline using a
  # macro:
  sky_append_flag(SKY_HAS_D__EXTERN_ALWAYS_INLINE -D__extern_always_inline=inline)
endif()

if(SKY_CXX_COMPILER_CLANG)
  if (skyr_BUILD_WITH_LLVM_LIBCXX)
    sky_append_flag(SKY_HAS_LLVM_LIBCXX "-stdlib=libc++")
  else()
    sky_append_flag(SKY_HAS_LIBSTDCXX "-stdlib=libstdc++")
  endif()
endif()

# Template diagnostic flags
sky_append_flag(SKY_HAS_FDIAGNOSTIC_SHOW_TEMPLATE_TREE -fdiagnostics-show-template-tree)
sky_append_flag(SKY_HAS_FTEMPLATE_BACKTRACE_LIMIT "-ftemplate-backtrace-limit=0")
sky_append_flag(SKY_HAS_FMACRO_BACKTRACE_LIMIT "-fmacro-backtrace-limit=1")

if (SKY_VERBOSE_BUILD)
  get_directory_property(RANGES_COMPILE_OPTIONS COMPILE_OPTIONS)
  message(STATUS "[skyr-url]: C++ flags: ${CMAKE_CXX_FLAGS}")
  message(STATUS "[skyr-url]: C++ debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
  message(STATUS "[skyr-url]: C++ Release Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
  message(STATUS "[skyr-url]: C++ Compile Flags: ${CMAKE_CXX_COMPILE_FLAGS}")
  message(STATUS "[skyr-url]: Compile options: ${RANGES_COMPILE_OPTIONS}")
  message(STATUS "[skyr-url]: C Flags: ${CMAKE_C_FLAGS}")
  message(STATUS "[skyr-url]: C Compile Flags: ${CMAKE_C_COMPILE_FLAGS}")
  message(STATUS "[skyr-url]: EXE Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
  message(STATUS "[skyr-url]: C++ Linker flags: ${CMAKE_CXX_LINK_FLAGS}")
  message(STATUS "[skyr-url]: MODULE Linker flags: ${CMAKE_MODULE_LINKER_FLAGS}")
  get_directory_property(CMakeCompDirDefs COMPILE_DEFINITIONS)
  message(STATUS "[skyr-url]: Compile Definitions: ${CmakeCompDirDefs}")
endif()
