# Benchmarks for skyr-url

add_executable(url_parsing_bench url_parsing_bench.cpp)

target_link_libraries(
        url_parsing_bench
        PRIVATE
        skyr-url
)

target_compile_features(url_parsing_bench PRIVATE cxx_std_23)

# Add compiler-specific optimizations for benchmarking
target_compile_options(
        url_parsing_bench
        PRIVATE
        $<${gnu}:-O3>
        $<${gnu}:-march=native>
        $<${clang}:-O3>
        $<${clang}:-march=native>
        $<${msvc}:/O2>
)