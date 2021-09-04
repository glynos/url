#################################################
# skyr-core
#################################################

add_library(skyr-core-v3 STATIC)

target_sources(skyr-core-v3 PRIVATE
        ../../skyr/v3/core/check_input.ixx
        ../../skyr/v3/core/url_parse_errc.ixx
        ../../skyr/v3/core/host.ixx
        ../../skyr/v3/core/parse.ixx
        ../../skyr/v3/core/parse_path.ixx
        ../../skyr/v3/core/parse_query.ixx
        ../../skyr/v3/core/schemes.ixx
        ../../skyr/v3/core/serialize.ixx
        ../../skyr/v3/core/url_parse_state.ixx
        ../../skyr/v3/core/url_parser_context.ixx
        ../../skyr/v3/core/url_record.ixx
)

target_link_libraries(
        skyr-core-v3
        PUBLIC
        skyr-network-v3
        skyr-domain-v3
        skyr-percent_encoding-v3
        tl::expected
        $<${libcxx}:c++>
)
