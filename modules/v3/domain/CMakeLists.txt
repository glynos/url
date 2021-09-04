#################################################
# skyr-domain
#################################################

add_library(skyr-domain-v3 STATIC)

target_sources(skyr-domain-v3 PRIVATE
        ../../skyr/v3/domain/idna_tables.ixx
        ../../skyr/v3/domain/idna_status.ixx
        ../../skyr/v3/domain/idna.ixx
        ../../skyr/v3/domain/domain_errc.ixx
        ../../skyr/v3/domain/punycode.ixx
        ../../skyr/v3/domain/domain.ixx
)

target_link_libraries(
        skyr-domain-v3
        PUBLIC
        skyr-unicode-v3
        tl::expected
        $<${libcxx}:c++>
)
