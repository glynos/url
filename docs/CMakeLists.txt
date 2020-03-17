# Copyright (c) Glyn Matthews 2018-20.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(${Skyr_SOURCE_DIR}/cmake/SkyrFunctions.cmake)

LIST(APPEND CMAKE_MODULE_PATH "${Skyr_SOURCE_DIR}/cmake")

find_package(Doxygen REQUIRED)
find_package(Pandoc REQUIRED)
find_package(Sphinx REQUIRED)

set(DOC_FILES
        _build
        _static
        _templates
        index.rst
        unicode.rst
        idna.rst
        domain.rst
        encoding.rst
        encoding_u8_view.rst
        encoding_u16_view.rst
        encoding_byte_transform.rst
        encoding_u16_transform.rst
        encoding_u32_transform.rst
        url.rst
        url_class.rst
        url_record.rst
        url_error_codes.rst
        url_search_parameters.rst
        )

# Copy RST files
foreach(doc_file ${DOC_FILES})
    file(COPY ${doc_file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Generate API documentation using Doxygen
configure_file(
        Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

configure_file(
        conf.py.in
        ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)

add_custom_target(
        doxygen ALL
        COMMAND
            ${DOXYGEN}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generating documentation with Doxygen"
)

# Convert markdown to RST
SET(
        GENERATED_FROM_MARKDOWN
        readme.rst
        changelog.rst
)

foreach (file_name ${GENERATED_FROM_MARKDOWN})
    remove_extension(${file_name} basename)
    string(TOUPPER ${basename} basename_upper)
    set(target ${basename})
    string(PREPEND target "doc_")
    add_custom_target(
            ${target}
            pandoc ${Skyr_SOURCE_DIR}/${basename_upper}.md -f markdown -t rst -s -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            WORKING_DIRECTORY
                ${CMAKE_CURRENT_BINARY_DIR}
            BYPRODUCTS
                ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
            COMMENT
                "Generating RST file with pandoc" VERBATIM
    )
endforeach()

# Bring it all together using Sphinx
add_custom_target(
        doc ALL
        COMMAND
            ${SPHINX_EXECUTABLE} -M html ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            doxygen ${GENERATED_FROM_MARKDOWN}
        COMMENT
            "Generating documentation with Sphinx"
)
