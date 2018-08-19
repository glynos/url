# Header file `unicode.hpp`

``` cpp
namespace skyr
{
    enum class unicode_errc;

    std::error_code make_error_code(skyr::unicode_errc error);

    expected<std::string, std::error_code> wstring_to_bytes(std::wstring_view input);

    expected<std::wstring, std::error_code> wstring_from_bytes(std::string_view input);

    expected<std::string, std::error_code> utf16_to_bytes(std::u16string_view input);

    expected<std::u16string, std::error_code> utf16_from_bytes(std::string_view input);

    expected<std::u32string, std::error_code> utf32_from_bytes(std::string_view input);

    expected<std::string, std::error_code> utf32_to_bytes(std::u32string_view input);
}
```

### Enumeration `skyr::unicode_errc`

``` cpp
enum class unicode_errc
{
    overflow,
    illegal_byte_sequence
};
```

Enumerates Unicode errors.

-----

### Function `skyr::make_error_code`

``` cpp
std::error_code make_error_code(skyr::unicode_errc error);
```

Creates a `std::error_code` given a `skyr::unicode_errc` value.

### Parameter `skyr::error`

``` cpp
skyr::unicode_errc error
```

A Unicode error.

*Returns:* A `std::error_code` object.

-----

-----

### Function `skyr::wstring_to_bytes`

``` cpp
expected<std::string, std::error_code> wstring_to_bytes(std::wstring_view input);
```

Converts a `std::wstring` (assuming UTF-16) string to UTF-8.

### Parameter `skyr::input`

``` cpp
std::wstring_view input
```

A UTF-16 string.

*Returns:* A UTF-8 `std::string` or an error on failure.

-----

-----

### Function `skyr::wstring_from_bytes`

``` cpp
expected<std::wstring, std::error_code> wstring_from_bytes(std::string_view input);
```

Converts a `std::string` (assuming UTF-8) string to UTF-16.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

A UTF-8 string.

*Returns:* A UTF-16 `std::wstring` or an error on failure.

-----

-----

### Function `skyr::utf16_to_bytes`

``` cpp
expected<std::string, std::error_code> utf16_to_bytes(std::u16string_view input);
```

Converts a `std::u16string` string to UTF-8.

### Parameter `skyr::input`

``` cpp
std::u16string_view input
```

A UTF-16 string.

*Returns:* A UTF-8 `std::string` or an error on failure.

-----

-----

### Function `skyr::utf16_from_bytes`

``` cpp
expected<std::u16string, std::error_code> utf16_from_bytes(std::string_view input);
```

Converts a `std::string` (assuming UTF-8) string to UTF-16.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

A UTF-8 string.

*Returns:* A UTF-16 `std::u16string` or an error on failure.

-----

-----

### Function `skyr::utf32_from_bytes`

``` cpp
expected<std::u32string, std::error_code> utf32_from_bytes(std::string_view input);
```

Converts a `std::u32string` string to UTF-8.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

A UTF-32 string.

*Returns:* A UTF-8 `std::string` or an error on failure.

-----

-----

### Function `skyr::utf32_to_bytes`

``` cpp
expected<std::string, std::error_code> utf32_to_bytes(std::u32string_view input);
```

Converts a `std::string` (assuming UTF-8) string to UTF-32.

### Parameter `skyr::input`

``` cpp
std::u32string_view input
```

A UTF-8 string.

*Returns:* A UTF-32 `std::u32string` or an error on failure.

-----

-----
