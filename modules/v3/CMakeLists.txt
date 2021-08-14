#################################################
# skyr-url
#################################################

add_library(skyr-url-v3 INTERFACE)

set(clang $<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>)
set(libcxx $<AND:${clang},$<BOOL:${skyr_BUILD_WITH_LLVM_LIBCXX}>>)

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
