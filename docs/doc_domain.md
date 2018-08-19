# Header file `domain.hpp`

``` cpp
namespace skyr
{
    enum class domain_errc;

    std::error_code make_error_code(skyr::domain_errc error);

    expected<std::string, std::error_code> punycode_encode(std::string_view input);

    expected<std::string, std::error_code> punycode_encode(std::u32string_view input);

    expected<std::string, std::error_code> punycode_decode(std::string_view input);

    expected<std::string, std::error_code> domain_to_ascii(std::string_view domain, bool be_strict = true);

    expected<std::string, std::error_code> domain_to_ascii(std::u32string_view domain, bool be_strict = true);
}
```

### Enumeration `skyr::domain_errc`

``` cpp
enum class domain_errc
{
    disallowed_code_point = 1,
    bad_input,
    overflow,
    encoding_error
};
```

Enumerates domain processing errors.

#### Enumerators

  - `disallowed_code_point` - The domain code point is disallowed
  - `bad_input` - The encoder or decoder received bad input
  - `overflow` - Overflow
  - `encoding_error` - Unicode encoding error

-----

### Function `skyr::make_error_code`

``` cpp
std::error_code make_error_code(skyr::domain_errc error);
```

Creates a `std::error_code` given a `skyr::domain_errc` value.

### Parameter `skyr::error`

``` cpp
skyr::domain_errc error
```

A domain error.

*Returns:* A `std::error_code` object.

-----

-----

### Function `skyr::punycode_encode`

``` cpp
expected<std::string, std::error_code> punycode_encode(std::string_view input);
```

Performs punycode encoding based on a reference implementation defined in [RFC 3492](https://tools.ietf.org/html/rfc3492).

### Parameter `skyr::input`

``` cpp
std::string_view input
```

A UTF-8 encoded domain to be encoded.

*Returns:* The encoded ASCII domain, or an error.

-----

-----

### Function `skyr::punycode_encode`

``` cpp
expected<std::string, std::error_code> punycode_encode(std::u32string_view input);
```

Performs punycode encoding based on a reference implementation defined in [RFC 3492](https://tools.ietf.org/html/rfc3492).

### Parameter `skyr::input`

``` cpp
std::u32string_view input
```

A UTF-32 encoded domain to be encoded.

*Returns:* The encoded ASCII domain, or an error.

-----

-----

### Function `skyr::punycode_decode`

``` cpp
expected<std::string, std::error_code> punycode_decode(std::string_view input);
```

Performs punycode decoding based on a reference implementation defined in [RFC 3492](https://tools.ietf.org/html/rfc3492).

### Parameter `skyr::input`

``` cpp
std::string_view input
```

An ASCII encoded domain to be decoded.

*Returns:* The decoded UTF-8 domain, or an error.

-----

-----

### Function `skyr::domain_to_ascii`

``` cpp
expected<std::string, std::error_code> domain_to_ascii(std::string_view domain, bool be_strict = true);
```

Converts a UTF-8 encoded domain to ASCII using [IDNA processing](https://www.unicode.org/reports/tr46/#Processing).

#### Parameters

  - `domain` - A domain.

### Parameter `skyr::be_strict`

``` cpp
bool be_strict = true
```

Tells the processor to be strict.

returns An ASCII domain, or an error.

-----

-----

### Function `skyr::domain_to_ascii`

``` cpp
expected<std::string, std::error_code> domain_to_ascii(std::u32string_view domain, bool be_strict = true);
```

Converts a UTF-32 encoded domain to ASCII using [IDNA processing](https://www.unicode.org/reports/tr46/#Processing).

#### Parameters

  - `domain` - A domain.

### Parameter `skyr::be_strict`

``` cpp
bool be_strict = true
```

Tells the processor to be strict.

returns An ASCII domain, or an error.

-----

-----
