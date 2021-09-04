#################################################
# skyr-unicode
#################################################

add_library(skyr-unicode-v3 STATIC)

target_sources(skyr-unicode-v3 PRIVATE
        ../../skyr/v3/unicode/unicode_errc.ixx
        ../../skyr/v3/unicode/core.ixx
        ../../skyr/v3/unicode/constants.ixx
        ../../skyr/v3/unicode/code_points/code_point_u8.ixx
        ../../skyr/v3/unicode/code_points/code_point_u16.ixx
        ../../skyr/v3/unicode/code_point.ixx
        ../../skyr/v3/unicode/traits/iterator_value.ixx
        ../../skyr/v3/unicode/traits/range_iterator.ixx
        ../../skyr/v3/unicode/traits/range_value.ixx
        ../../skyr/v3/unicode/ranges/sentinel.ixx
        ../../skyr/v3/unicode/ranges/transforms/transform_u8.ixx
        ../../skyr/v3/unicode/ranges/transforms/transform_u16.ixx
        ../../skyr/v3/unicode/ranges/transforms/transform_u32.ixx
        ../../skyr/v3/unicode/ranges/views/view_unchecked_u8.ixx
        ../../skyr/v3/unicode/ranges/views/view_u8.ixx
        ../../skyr/v3/unicode/ranges/views/view_u16.ixx
        ../../skyr/v3/unicode/u8_string.ixx
)

target_link_libraries(
        skyr-unicode-v3
        PUBLIC
        skyr-concepts-v3
        tl::expected
        $<${libcxx}:c++>
)
