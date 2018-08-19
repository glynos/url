# Header file `ipv6_address.hpp`

``` cpp
#define SKYR_IPV6_ADDRESS_INC

namespace skyr
{
    enum class ipv6_address_errc;

    std::error_code make_error_code(skyr::ipv6_address_errc error);

    class ipv6_address;

    expected<skyr::ipv6_address, std::error_code> parse_ipv6_address(std::string_view input);
}
```

### Enumeration `skyr::ipv6_address_errc`

``` cpp
enum class ipv6_address_errc
{
    validation_error
};
```

Enumerates IPv6 address parsing errors.

-----

### Function `skyr::make_error_code`

``` cpp
std::error_code make_error_code(skyr::ipv6_address_errc error);
```

Creates a `std::error_code` given a `skyr::ipv6_address_errc` value.

### Parameter `skyr::error`

``` cpp
skyr::ipv6_address_errc error
```

An IPv6 address error.

*Returns:* A `std::error_code` object.

-----

-----

### Class `skyr::ipv6_address`

``` cpp
class ipv6_address
{
public:
    ipv6_address();

    explicit ipv6_address(std::array<unsigned short, 8> address);

    std::string to_string() const;
};
```

Represents an IPv6 address.

### Constructor `skyr::ipv6_address::ipv6_address`

``` cpp
ipv6_address();
```

Constructor.

-----

### Constructor `skyr::ipv6_address::ipv6_address`

``` cpp
explicit ipv6_address(std::array<unsigned short, 8> address);
```

Constructor.

#### Parameters

  - `address` - Sets the IPv6 address to `address`.

-----

### Function `skyr::ipv6_address::to_string`

``` cpp
std::string to_string() const;
```

*Returns:* The IPv4 address as a string.

-----

-----

### Function `skyr::parse_ipv6_address`

``` cpp
expected<skyr::ipv6_address, std::error_code> parse_ipv6_address(std::string_view input);
```

Parses an IPv6 address.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

An input string.

*Returns:* An `ipv6_address` object or an error.

-----

-----
