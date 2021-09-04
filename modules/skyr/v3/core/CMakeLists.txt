#################################################
# skyr-core
#################################################

add_library(skyr-core-v3 STATIC)

target_sources(skyr-core-v3 PRIVATE
        check_input.ixx
        url_parse_errc.ixx
        host.ixx
        parse.ixx
        parse_path.ixx
        parse_query.ixx
        schemes.ixx
        serialize.ixx
        url_parse_state.ixx
        url_parser_context.ixx
        url_record.ixx
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
