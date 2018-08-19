# Header file `url_parse.hpp`

``` cpp
#define SKYR_URL_PARSE_INC

namespace skyr
{
    expected<skyr::url_record, std::error_code> parse(std::string input, optional<skyr::url_record> const& base = nullopt);
}
```

### Function `skyr::parse`

``` cpp
expected<skyr::url_record, std::error_code> parse(std::string input, optional<skyr::url_record> const& base = nullopt);
```

Parses a URL and returns a `url_record`.

#### Parameters

  - `input` - The input string.

### Parameter `skyr::base`

``` cpp
optional<skyr::url_record> const& base = nullopt
```

An optional base URL.

*Returns:* A `url_record` on success and an error code on failure.

-----

-----
