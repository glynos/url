# Project index

  - [`SKYR_EXPECTED_INC`](doc_expected.html#standardese-expected-hpp)

  - [`SKYR_IPV4_ADDRESS_INC`](doc_ipv4_address.html#standardese-ipv4_address-hpp)

  - [`SKYR_IPV6_ADDRESS_INC`](doc_ipv6_address.html#standardese-ipv6_address-hpp)

  - [`SKYR_OPTIONAL_INC`](doc_optional.html#standardese-optional-hpp)

  - [`SKYR_URL_ERROR_INC`](doc_url_error.html#standardese-url_error-hpp)

  - [`SKYR_URL_INC`](doc_url.html#standardese-url-hpp)

  - [`SKYR_URL_PARSE_INC`](doc_url_parse.html#standardese-url_parse-hpp)

  - [`SKYR_URL_PARSE_STATE_INC`](doc_url_parse_state.html#standardese-url_parse_state-hpp)

  - [`SKYR_URL_PERCENT_ENCODE_INC`](doc_percent_encode.html#standardese-percent_encode-hpp)

  - [`SKYR_URL_RECORD_INC`](doc_url_record.html#standardese-url_record-hpp)

  - [`SKYR_URL_SEARCH_PARAMETERS_INC`](doc_url_search_parameters.html#standardese-url_search_parameters-hpp)

  - [`SKYR_URL_SERIALIZE_INC`](doc_url_serialize.html#standardese-url_serialize-hpp)

  - ## Namespace `skyr`
    
      - [`bad_expected_access`](doc_expected.html#standardese-skyr)
    
      - [`bad_optional_access`](doc_optional.html#standardese-skyr)
    
      - [`c0_control_set`](doc_percent_encode.html#standardese-skyr__c0_control_set) - Defines code points in the c0 control percent-encode set.
    
      - [`domain_errc`](doc_domain.html#standardese-skyr__domain_errc) - Enumerates domain processing errors.
    
      - [`domain_to_ascii`](doc_domain.html#standardese-skyr__domain_to_ascii-std__string_view-bool-) - Converts a UTF-8 encoded domain to ASCII using [IDNA processing](https://www.unicode.org/reports/tr46/#Processing).
    
      - [`exclude_set`](doc_percent_encode.html#standardese-skyr__exclude_set) - Exclude code point set when percent encoding.
    
      - [`expected`](doc_expected.html#standardese-skyr)
    
      - [`fragment_set`](doc_percent_encode.html#standardese-skyr__fragment_set) - Defines code points in the fragment percent-encode set.
    
      - [`ipv4_address`](doc_ipv4_address.html#standardese-skyr__ipv4_address) - Represents an IPv4 address.
    
      - [`ipv4_address_errc`](doc_ipv4_address.html#standardese-skyr__ipv4_address_errc) - Enumerates IPv4 address parsing errors.
    
      - [`ipv6_address`](doc_ipv6_address.html#standardese-skyr__ipv6_address) - Represents an IPv6 address.
    
      - [`ipv6_address_errc`](doc_ipv6_address.html#standardese-skyr__ipv6_address_errc) - Enumerates IPv6 address parsing errors.
    
      - [`is_percent_encoded`](doc_percent_encode.html#standardese-skyr__is_percent_encoded-std__string_view-std__localeconst--) - Tests whether the input string contains percent encoded values.
    
      - [`make_error_code`](doc_ipv4_address.html#standardese-skyr__make_error_code-skyr__ipv4_address_errc-) - Creates a `std::error_code` given a `skyr::ipv4_address_errc` value.
    
      - [`make_unexpected`](doc_expected.html#standardese-skyr)
    
      - [`make_url`](doc_url.html#standardese-skyr__make_url-Source--url__string_type---) - Parses a URL string and constructs a `url` object.
    
      - [`nullopt`](doc_optional.html#standardese-skyr)
    
      - [`operator!=`](doc_url.html#standardese-skyr__operator---skyr__urlconst--skyr__urlconst--) - Tests two URLs for inequality.
    
      - [`operator<`](doc_url.html#standardese-skyr__operator--skyr__urlconst--skyr__urlconst--) - Comparison operator
    
      - [`operator<=`](doc_url.html#standardese-skyr__operator---skyr__urlconst--skyr__urlconst--) - Comparison operator
    
      - [`operator==`](doc_url.html#standardese-skyr__operator---skyr__urlconst--skyr__urlconst--) - Tests two URLs for equality according to the [WhatWG specification](https://url.spec.whatwg.org/#url-equivalence).
    
      - [`operator>`](doc_url.html#standardese-skyr__operator--skyr__urlconst--skyr__urlconst--) - Comparison operator
    
      - [`operator>=`](doc_url.html#standardese-skyr__operator---skyr__urlconst--skyr__urlconst--) - Comparison operator
    
      - [`optional`](doc_optional.html#standardese-skyr)
    
      - [`parse`](doc_url_parse.html#standardese-skyr__parse-std__string-optional-skyr__url_record-const--) - Parses a URL and returns a `url_record`.
    
      - [`parse_ipv4_address`](doc_ipv4_address.html#standardese-skyr__parse_ipv4_address-std__string_view-) - Parses an IPv4 address.
    
      - [`parse_ipv6_address`](doc_ipv6_address.html#standardese-skyr__parse_ipv6_address-std__string_view-) - Parses an IPv6 address.
    
      - [`path_set`](doc_percent_encode.html#standardese-skyr__path_set) - Defines code points in the path percent-encode set.
    
      - [`percent_decode`](doc_percent_encode.html#standardese-skyr__percent_decode-std__string_view-) - Percent decodes a string.
    
      - [`percent_decode_byte`](doc_percent_encode.html#standardese-skyr__percent_decode_byte-std__string_view-) - Percent decode an already encoded string into a byte value.
    
      - [`percent_encode`](doc_percent_encode.html#standardese-skyr__percent_encode-std__string_view-skyr__exclude_setconst--) - Percent encode a string.
    
      - [`percent_encode_byte`](doc_percent_encode.html#standardese-skyr__percent_encode_byte-char-skyr__exclude_setconst--) - Percent encode a byte if it is not in the exclude set.
    
      - [`percent_encode_errc`](doc_percent_encode.html#standardese-skyr__percent_encode_errc) - Enumerates percent encoding errors.
    
      - [`punycode_decode`](doc_domain.html#standardese-skyr__punycode_decode-std__string_view-) - Performs punycode decoding based on a reference implementation defined in [RFC 3492](https://tools.ietf.org/html/rfc3492).
    
      - [`punycode_encode`](doc_domain.html#standardese-skyr__punycode_encode-std__string_view-) - Performs punycode encoding based on a reference implementation defined in [RFC 3492](https://tools.ietf.org/html/rfc3492).
    
      - [`query_set`](doc_percent_encode.html#standardese-skyr__query_set) - Defines code points in the fragment percent-encode set and U+0027 (’).
    
      - [`serialize`](doc_url_serialize.html#standardese-skyr__serialize-skyr__url_recordconst--bool-) - Serializes a URL record according to the [WhatWG specification](https://url.spec.whatwg.org/#url-serializing).
    
      - [`swap`](doc_url_record.html#standardese-skyr__swap-skyr__url_record--skyr__url_record--) - Swaps to `url_record` objects.
    
      - [`unexpected`](doc_expected.html#standardese-skyr)
    
      - [`unicode_errc`](doc_unicode.html#standardese-skyr__unicode_errc) - Enumerates Unicode errors.
    
      - [`url`](doc_url.html#standardese-skyr__url) - Represents a URL. Parsing is performed according to the [WhatWG specification](https://url.spec.whatwg.org/).
    
      - [`url_parse_errc`](doc_url_error.html#standardese-skyr__url_parse_errc) - Enumerates URL parser errors.
    
      - [`url_parse_error`](doc_url.html#standardese-skyr__url_parse_error) - Thrown when there is an error parsing the URL.
    
      - [`url_parse_state`](doc_url_parse_state.html#standardese-skyr__url_parse_state) - States of the URL parser.
    
      - [`url_record`](doc_url_record.html#standardese-skyr__url_record) - Represents the parts of a URL identifier.
    
      - [`url_search_parameters`](doc_url_search_parameters.html#standardese-skyr__url_search_parameters) - Allows iterating through search parameters.
    
      - [`userinfo_set`](doc_percent_encode.html#standardese-skyr__userinfo_set) - Defines code points in the userinfo percent-encode set.
    
      - [`utf16_from_bytes`](doc_unicode.html#standardese-skyr__utf16_from_bytes-std__string_view-) - Converts a `std::string` (assuming UTF-8) string to UTF-16.
    
      - [`utf16_to_bytes`](doc_unicode.html#standardese-skyr__utf16_to_bytes-std__u16string_view-) - Converts a `std::u16string` string to UTF-8.
    
      - [`utf32_from_bytes`](doc_unicode.html#standardese-skyr__utf32_from_bytes-std__string_view-) - Converts a `std::u32string` string to UTF-8.
    
      - [`utf32_to_bytes`](doc_unicode.html#standardese-skyr__utf32_to_bytes-std__u32string_view-) - Converts a `std::string` (assuming UTF-8) string to UTF-32.
    
      - [`wstring_from_bytes`](doc_unicode.html#standardese-skyr__wstring_from_bytes-std__string_view-) - Converts a `std::string` (assuming UTF-8) string to UTF-16.
    
      - [`wstring_to_bytes`](doc_unicode.html#standardese-skyr__wstring_to_bytes-std__wstring_view-) - Converts a `std::wstring` (assuming UTF-16) string to UTF-8.
