#################################################
# skyr-network
#################################################

add_library(skyr-network-v3 STATIC)

target_sources(skyr-network-v3 PRIVATE
        ../../skyr/v3/network/ipv4_address_errc.ixx
        ../../skyr/v3/network/ipv4.ixx
        ../../skyr/v3/network/ipv6_address_errc.ixx
        ../../skyr/v3/network/ipv6.ixx
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
