# Copyright (c) Glyn Matthews 2012-2021.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


set(full_warnings $<BOOL:${skyr_FULL_WARNINGS}>)
set(warnings_as_errors $<BOOL:${skyr_WARNINGS_AS_ERRORS}>)
set(no_exceptions $<BOOL:${skyr_BUILD_WITHOUT_EXCEPTIONS}>)
set(no_rtti $<BOOL:${skyr_BUILD_WITHOUT_RTTI}>)

set(gnu $<CXX_COMPILER_ID:GNU>)
set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)
set(msvc $<CXX_COMPILER_ID:MSVC>)
set(clang_with_gnu_stdlib $<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<BOOL:${WIN32}>>,$<NOT:$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>>)

function(skyr_create_test file_name output_dir test_name version)
    skyr_remove_extension(${file_name} base)
    set(test ${base}-${version})
    add_executable(${test})
    target_sources(${test} PRIVATE ${base}.cpp)
    if (skyr_BUILD_V3)
        target_compile_definitions(${test} PRIVATE $<${msvc}:SEQUENTIAL_PROCESSING=0>)
    endif()
    target_compile_options(${test} PRIVATE $<${libcxx}:-stdlib=libc++>)
    add_dependencies(${test} skyr-url-${version})
    target_link_libraries(
            ${test}
            PRIVATE
            skyr-url-${version}
            Catch2::Catch2
            fmt::fmt
    )
    set_target_properties(
        ${test}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
    )
    if (skyr_BUILD_V3)
        set_target_properties(
                ${test}
                PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION ON
        )
    endif()
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
