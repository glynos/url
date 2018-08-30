# Header file `percent_encode.hpp`

``` cpp
#define SKYR_URL_PERCENT_ENCODE_INC

namespace skyr
{
    enum class percent_encode_errc;

    std::error_code make_error_code(skyr::percent_encode_errc error);

    class exclude_set;

    class c0_control_set;

    class fragment_set;

    class query_set;

    class path_set;

    class userinfo_set;

    std::string percent_encode_byte(char in, skyr::exclude_set const& excludes = c0_control_set();

    expected<std::string, std::error_code> percent_encode(std::string_view input, skyr::exclude_set const& excludes = c0_control_set();

    expected<std::string, std::error_code> percent_encode(std::u32string_view input, skyr::exclude_set const& excludes = c0_control_set();

    expected<char, std::error_code> percent_decode_byte(std::string_view input);

    expected<std::string, std::error_code> percent_decode(std::string_view input);

    bool is_percent_encoded(std::string_view input, std::locale const& locale = std::locale::classic();
}
```

### Enumeration `skyr::percent_encode_errc`

``` cpp
enum class percent_encode_errc
{
    non_hex_input,
    overflow
};
```

Enumerates percent encoding errors.

#### Enumerators

  - `non_hex_input` - Input was not a hex value.
  - `overflow` - Overflow.

-----

### Function `skyr::make_error_code`

``` cpp
std::error_code make_error_code(skyr::percent_encode_errc error);
```

Creates a `std::error_code` given a `skyr::percent_encode_errc` value.

### Parameter `skyr::error`

``` cpp
skyr::percent_encode_errc error
```

A percent encoding error.

*Returns:* A `std::error_code` object.

-----

-----

### Class `skyr::exclude_set`

``` cpp
class exclude_set
{
protected:
    virtual ~exclude_set();

public:
    virtual bool contains_impl(char in) const = 0;
};
```

Exclude code point set when percent encoding.

### Function `skyr::exclude_set::contains_impl`

``` cpp
virtual bool contains_impl(char in) const = 0;
```

Tests whether the byte is in the excluded set.

### Parameter `skyr::exclude_set::in`

``` cpp
char in
```

Input byte.

*Returns:* `true` if `in` is in the excluded set, `c` false otherwise.

-----

-----

-----

### Class `skyr::c0_control_set`

``` cpp
class c0_control_set
: public exclude_set
{
public:
    virtual ~c0_control_set();

    virtual bool contains_impl(char in) const;
};
```

Defines code points in the c0 control percent-encode set.

-----

### Class `skyr::fragment_set`

``` cpp
class fragment_set
: public exclude_set
{
public:
    virtual ~fragment_set();

    virtual bool contains_impl(char in) const;
};
```

Defines code points in the fragment percent-encode set.

-----

### Class `skyr::query_set`

``` cpp
class query_set
: public exclude_set
{
public:
    virtual ~query_set();

    virtual bool contains_impl(char in) const;
};
```

Defines code points in the fragment percent-encode set and U+0027 (’).

-----

### Class `skyr::path_set`

``` cpp
class path_set
: public exclude_set
{
public:
    virtual ~path_set();

    virtual bool contains_impl(char in) const;
};
```

Defines code points in the path percent-encode set.

-----

### Class `skyr::userinfo_set`

``` cpp
class userinfo_set
: public exclude_set
{
public:
    virtual ~userinfo_set();

    virtual bool contains_impl(char in) const;
};
```

Defines code points in the userinfo percent-encode set.

-----

### Function `skyr::percent_encode_byte`

``` cpp
std::string percent_encode_byte(char in, skyr::exclude_set const& excludes = c0_control_set();
```

Percent encode a byte if it is not in the exclude set.

#### Parameters

  - `in` - The input byte.

### Parameter `skyr::excludes`

``` cpp
skyr::exclude_set const& excludes = c0_control_set(
```

The set of code points to exclude when percent encoding.

*Returns:* A percent encoded string if `in` is not in the exclude set, `in` as a string otherwise.

-----

-----

### Function `skyr::percent_encode`

``` cpp
expected<std::string, std::error_code> percent_encode(std::string_view input, skyr::exclude_set const& excludes = c0_control_set();
```

Percent encode a string.

#### Parameters

  - `input` - A string of bytes.

### Parameter `skyr::excludes`

``` cpp
skyr::exclude_set const& excludes = c0_control_set(
```

The set of code points to exclude when percent encoding.

*Returns:* A percent encoded ASCII string, or an error on failure.

-----

-----

### Function `skyr::percent_encode`

``` cpp
expected<std::string, std::error_code> percent_encode(std::u32string_view input, skyr::exclude_set const& excludes = c0_control_set();
```

Percent encode a string.

#### Parameters

  - `input` - A UTF-32 string.

### Parameter `skyr::excludes`

``` cpp
skyr::exclude_set const& excludes = c0_control_set(
```

The set of code points to exclude when percent encoding.

*Returns:* A percent encoded ASCII string, or an error on failure.

-----

-----

### Function `skyr::percent_decode_byte`

``` cpp
expected<char, std::error_code> percent_decode_byte(std::string_view input);
```

Percent decode an already encoded string into a byte value.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

An string of the for %XX, where X is a hexadecimal value/

*Returns:* The percent decoded byte, or an error on failure.

-----

-----

### Function `skyr::percent_decode`

``` cpp
expected<std::string, std::error_code> percent_decode(std::string_view input);
```

Percent decodes a string.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

An ASCII string.

*Returns:* A UTF-8 string, or an error on failure.

-----

-----

### Function `skyr::is_percent_encoded`

``` cpp
bool is_percent_encoded(std::string_view input, std::locale const& locale = std::locale::classic();
```

Tests whether the input string contains percent encoded values.

#### Parameters

  - `input` - A string.

### Parameter `skyr::locale`

``` cpp
std::locale const& locale = std::locale::classic(
```

A locale.

*Returns:* `true` if the input string contains percent encoded values, `false` otherwise.

-----

-----
