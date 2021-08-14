#################################################
# skyr-url
#################################################

add_library(skyr-url-v3 INTERFACE)

set(full_warnings $<BOOL:${skyr_FULL_WARNINGS}>)
set(warnings_as_errors $<BOOL:${skyr_WARNINGS_AS_ERRORS}>)
set(no_exceptions $<BOOL:${skyr_BUILD_WITHOUT_EXCEPTIONS}>)
set(no_rtti $<BOOL:${skyr_BUILD_WITHOUT_RTTI}>)

set(gnu $<CXX_COMPILER_ID:GNU>)
set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)
set(msvc $<CXX_COMPILER_ID:MSVC>)
set(clang_with_gnu_stdlib $<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<BOOL:${WIN32}>>,$<NOT:$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>>)

target_link_libraries(
        skyr-url-v3
        INTERFACE
        tl::expected
        range-v3
        $<${libcxx}:c++>
)

target_include_directories(
        skyr-url-v3
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
)
