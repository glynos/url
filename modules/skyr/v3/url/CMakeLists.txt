#################################################
# skyr-url
#################################################

add_library(skyr-url-v3 STATIC)

target_sources(skyr-url-v3 PRIVATE
        url.ixx
        url_search_parameters.ixx
)

target_link_libraries(
        skyr-url-v3
        PUBLIC
        skyr-core-v3
        tl::expected
        $<${libcxx}:c++>
)
