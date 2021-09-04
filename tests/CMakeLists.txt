# Copyright (c) Glyn Matthews 2012-2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

# Test dependencies
find_package(Catch2 CONFIG REQUIRED)

# Implementation dependencies
if (${skyr_BUILD_V1} OR ${skyr_BUILD_V2})
    find_package(fmt CONFIG REQUIRED)
endif()

function(skyr_create_test file_name output_dir test_name version)
    skyr_remove_extension(${file_name} base)
    set(test ${base}-${version})
    add_executable(${test})
    target_sources(${test} PRIVATE ${base}.cpp)
    target_compile_definitions(${test} PRIVATE $<$<AND:$<BOOL:${skyr_BUILD_V3}>,${msvc}>:SEQUENTIAL_PROCESSING=0>)
    target_compile_options(${test} PRIVATE $<${libcxx}:-stdlib=libc++> $<$<AND:$<BOOL:${skyr_BUILD_V3}>,${msvc}>:/experimental:module>)
    add_dependencies(${test} skyr-url-${version})
    target_link_libraries(
            ${test}
            PRIVATE
            skyr-url-${version}
            Catch2::Catch2
            $<$<OR:$<BOOL:${skyr_BUILD_V1}>,$<BOOL:${skyr_BUILD_V2}>>:fmt::fmt>
    )
    set_target_properties(
        ${test}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
        INTERPROCEDURAL_OPTIMIZATION $<BOOL:${skyr_BUILD_V3}>:ON>
    )
    add_test(${test} ${output_dir}/${test})
    set(test_name ${test} PARENT_SCOPE)
endfunction()

function(skyr_create_v3_test file_name output_dir test_name)
    skyr_remove_extension(${file_name} base)
    set(test ${base}-v3)
    add_executable(${test})
    target_sources(${test} PRIVATE ${base}.cpp)
    target_compile_definitions(${test} PRIVATE $<$<AND:$<BOOL:${skyr_BUILD_V3}>,${msvc}>:SEQUENTIAL_PROCESSING=0>)
    target_compile_options(${test} PRIVATE $<${libcxx}:-stdlib=libc++> $<$<AND:$<BOOL:${skyr_BUILD_V3}>,${msvc}>:/experimental:module>)
    target_link_libraries(
            ${test}
            PRIVATE
            Catch2::Catch2
    )
    set_target_properties(
        ${test}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
        INTERPROCEDURAL_OPTIMIZATION ON
    )
    add_test(${test} ${output_dir}/${test})
    set(test_name ${test} PARENT_SCOPE)
endfunction()

if (skyr_BUILD_V1)
    add_subdirectory(v1)
endif()

if (skyr_BUILD_V2)
    add_subdirectory(v2)
endif()

if (skyr_BUILD_V3)
    add_subdirectory(v3)
endif()
