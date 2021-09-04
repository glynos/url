#################################################
# skyr-percent_encoding
#################################################

add_library(skyr-percent_encoding-v3 STATIC)

target_sources(skyr-percent_encoding-v3 PRIVATE
        percent_encode_errc.ixx
        sentinel.ixx
        percent_decode.ixx
        percent_encode.ixx
)

target_link_libraries(
        skyr-percent_encoding-v3
        PUBLIC
        tl::expected
        $<${libcxx}:c++>
)
