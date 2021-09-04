#################################################
# skyr-network
#################################################

add_library(skyr-network-v3 STATIC)

target_sources(skyr-network-v3 PRIVATE
        ipv4_address_errc.ixx
        ipv4.ixx
        ipv6_address_errc.ixx
        ipv6.ixx
)

target_link_libraries(
        skyr-network-v3
        PUBLIC
        skyr-platform-v3
        skyr-containers-v3
        tl::expected
        $<${libcxx}:c++>

        PRIVATE
        range-v3
)
