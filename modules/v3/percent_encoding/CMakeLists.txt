#################################################
# skyr-percent_encoding
#################################################

add_library(skyr-percent_encoding-v3 STATIC)

target_sources(skyr-percent_encoding-v3 PRIVATE
        ../../skyr/v3/percent_encoding/percent_encode_errc.ixx
        ../../skyr/v3/percent_encoding/sentinel.ixx
        ../../skyr/v3/percent_encoding/percent_decode.ixx
        ../../skyr/v3/percent_encoding/percent_encode.ixx
)

target_link_libraries(
        skyr-percent_encoding-v3
        PUBLIC
        tl::expected
        $<${libcxx}:c++>
)
