#################################################
# skyr-platform
#################################################

add_library(skyr-platform-v3 STATIC)

target_sources(skyr-platform-v3 PRIVATE
        endianness.ixx
)

target_link_libraries(
        skyr-platform-v3
        PUBLIC
        $<${libcxx}:c++>
)
