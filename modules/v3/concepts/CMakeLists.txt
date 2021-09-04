#################################################
# skyr-concepts
#################################################

add_library(skyr-concepts-v3 STATIC)

target_sources(skyr-concepts-v3 PRIVATE
        ../../skyr/v3/concepts/url_concepts.ixx
)

target_link_libraries(
        skyr-concepts-v3
        PUBLIC
        $<${libcxx}:c++>
)
