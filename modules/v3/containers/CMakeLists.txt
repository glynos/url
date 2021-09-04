#################################################
# skyr-containers
#################################################

add_library(skyr-containers-v3 STATIC)

target_sources(skyr-containers-v3 PRIVATE
        ../../skyr/v3/containers/static_vector.ixx
        ../../skyr/v3/containers.ixx
)

target_link_libraries(
        skyr-containers-v3
        PUBLIC
        tl::expected
        $<${libcxx}:c++>

        PRIVATE
        range-v3
)
