# Copyright (c) Glyn Matthews 2025.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${PROJECT_SOURCE_DIR}/cmake/skyr-url-functions.cmake)

# Sanitizer tests - only build when sanitizers are enabled
if (NOT skyr_ENABLE_SANITIZERS)
    message(STATUS "Sanitizer tests skipped (enable with -Dskyr_ENABLE_SANITIZERS=ON)")
    return()
endif()

message(STATUS "Building sanitizer tests with AddressSanitizer + UndefinedBehaviorSanitizer")

foreach(
        file_name
        url_sanitizer_tests.cpp
)
    skyr_remove_extension(${file_name} basename)
    set(test ${basename})
    add_executable(${test} ${file_name})
    add_dependencies(${test} skyr-url)

    target_compile_options(
            ${test}
            PRIVATE
            # Standard warnings (but no -Werror for sanitizer tests)
            $<$<AND:$<OR:${gnu},${clang}>,${full_warnings}>:-Wall>
            $<$<AND:$<OR:${gnu},${clang}>,${no_exceptions}>:-fno-exceptions>
            $<$<AND:$<OR:${gnu},${clang}>,${no_rtti}>:-fno-rtti>
            $<${libcxx}:-stdlib=libc++>

            # AddressSanitizer flags (GCC/Clang)
            $<$<OR:${gnu},${clang}>:-fsanitize=address>
            $<$<OR:${gnu},${clang}>:-fsanitize=undefined>
            $<$<OR:${gnu},${clang}>:-fno-omit-frame-pointer>
            $<$<OR:${gnu},${clang}>:-fno-optimize-sibling-calls>
            $<$<OR:${gnu},${clang}>:-g>
            $<$<OR:${gnu},${clang}>:-O1>

            # MSVC sanitizer flags
            $<$<AND:${msvc},${full_warnings}>:/W4>
            $<$<AND:${msvc},$<NOT:${no_exceptions}>>:/EHsc>
            $<$<AND:${msvc},${no_rtti}>:/GR->
            $<${msvc}:/fsanitize=address>
            $<${msvc}:/Zi>
    )

    target_link_options(
            ${test}
            PRIVATE
            # Sanitizer linker flags (GCC/Clang)
            $<$<OR:${gnu},${clang}>:-fsanitize=address>
            $<$<OR:${gnu},${clang}>:-fsanitize=undefined>

            # MSVC sanitizer linker flags
            $<${msvc}:/fsanitize=address>
    )

    target_link_libraries(
            ${test}
            PRIVATE
            skyr-url
    )

    set_target_properties(
            ${test}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests/sanitizers/
    )

    # Add as a test so it can be run with ctest
    add_test(
            NAME ${test}
            COMMAND ${test}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests/sanitizers/
    )

    # Set environment variables for sanitizers
    set_tests_properties(
            ${test}
            PROPERTIES
            ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:check_initialization_order=1:strict_init_order=1:detect_stack_use_after_return=1"
    )
endforeach()