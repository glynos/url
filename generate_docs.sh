#!/usr/bin/env bash

../standardese/cmake-build-debug/tool/standardese \
    --config docs/standardese.config \
    --output.prefix=docs/ \
    --compilation.clang_binary=`which clang++` \
    include/skyr/domain.hpp \
    include/skyr/expected.hpp \
    include/skyr/ipv4_address.hpp \
    include/skyr/ipv6_address.hpp \
    include/skyr/optional.hpp \
    include/skyr/percent_encode.hpp \
    include/skyr/unicode.hpp \
    include/skyr/url.hpp \
    include/skyr/url_error.hpp \
    include/skyr/url_parse.hpp \
    include/skyr/url_parse_state.hpp \
    include/skyr/url_record.hpp \
    include/skyr/url_search_parameters.hpp \
    include/skyr/url_serialize.hpp
