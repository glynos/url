# Header file `url_error.hpp`

``` cpp
#define SKYR_URL_ERROR_INC

namespace skyr
{
    enum class url_parse_errc;

    std::error_code make_error_code(skyr::url_parse_errc error);
}
```

### Enumeration `skyr::url_parse_errc`

``` cpp
enum class url_parse_errc
{
    invalid_scheme_character = 1,
    not_an_absolute_url_with_fragment,
    empty_hostname,
    invalid_ipv4_address,
    invalid_ipv6_address,
    forbidden_host_point,
    cannot_decode_host_point,
    domain_error,
    cannot_be_a_base_url,
    cannot_have_a_username_password_or_port,
    invalid_port
};
```

Enumerates URL parser errors.

#### Enumerators

  - `invalid_scheme_character` - Character is not a valid scheme character.
  - `empty_hostname` - Hostname is empty.
  - `invalid_ipv4_address` - Invalid IPv4 address.
  - `invalid_ipv6_address` - Invalid IPv6 address.
  - `forbidden_host_point` - Character is a forbidden host point.
  - `cannot_decode_host_point` - Unable to decode host point.
  - `domain_error` - Invalid domain string.
  - `cannot_be_a_base_url` - URL cannot be a base URL.
  - `cannot_have_a_username_password_or_port` - URL cannot have a username, password or port.
  - `invalid_port` - Invalid port value.

-----

### Function `skyr::make_error_code`

``` cpp
std::error_code make_error_code(skyr::url_parse_errc error);
```

Creates a `std::error_code` given a `skyr::url_parse_errc` value.

### Parameter `skyr::error`

``` cpp
skyr::url_parse_errc error
```

A URL parse error.

*Returns:* A `std::error_code` object.

-----

-----
