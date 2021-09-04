#################################################
# skyr-domain
#################################################

add_library(skyr-domain-v3 STATIC)

target_sources(skyr-domain-v3 PRIVATE
        idna_tables.ixx
        idna_status.ixx
        idna.ixx
        domain_errc.ixx
        punycode.ixx
        domain.ixx
)

target_link_libraries(
        skyr-domain-v3
        PUBLIC
        skyr-unicode-v3
        tl::expected
        $<${libcxx}:c++>
)
