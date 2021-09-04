#################################################
# skyr-unicode
#################################################

add_library(skyr-unicode-v3 STATIC)

target_sources(skyr-unicode-v3 PRIVATE
        unicode_errc.ixx
        core.ixx
        constants.ixx
        code_points/code_point_u8.ixx
        code_points/code_point_u16.ixx
        code_point.ixx
        traits/iterator_value.ixx
        traits/range_iterator.ixx
        traits/range_value.ixx
        ranges/sentinel.ixx
        ranges/transforms/transform_u8.ixx
        ranges/transforms/transform_u16.ixx
        ranges/transforms/transform_u32.ixx
        ranges/views/view_unchecked_u8.ixx
        ranges/views/view_u8.ixx
        ranges/views/view_u16.ixx
        u8_string.ixx
)

target_link_libraries(
        skyr-unicode-v3
        PUBLIC
        skyr-concepts-v3
        tl::expected
        $<${libcxx}:c++>
)
