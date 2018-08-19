# Header file `ipv4_address.hpp`

``` cpp
#define SKYR_IPV4_ADDRESS_INC

namespace skyr
{
    enum class ipv4_address_errc;

    std::error_code make_error_code(skyr::ipv4_address_errc error);

    class ipv4_address;

    expected<skyr::ipv4_address, std::error_code> parse_ipv4_address(std::string_view input);
}
```

### Enumeration `skyr::ipv4_address_errc`

``` cpp
enum class ipv4_address_errc
{
    more_than_4_segments,
    empty_segment,
    invalid_segment_number,
    validation_error
};
```

Enumerates IPv4 address parsing errors.

#### Enumerators

  - `more_than_4_segments` - The input contains more than 4 segments.
  - `empty_segment` - The input contains an empty segment.
  - `invalid_segment_number` - The segment numers invalid.

-----

### Function `skyr::make_error_code`

``` cpp
std::error_code make_error_code(skyr::ipv4_address_errc error);
```

Creates a `std::error_code` given a `skyr::ipv4_address_errc` value.

### Parameter `skyr::error`

``` cpp
skyr::ipv4_address_errc error
```

An IPv4 address error.

*Returns:* A `std::error_code` object.

-----

-----

### Class `skyr::ipv4_address`

``` cpp
class ipv4_address
{
public:
    ipv4_address();

    explicit ipv4_address(unsigned int address);

    unsigned int address() const noexcept;

    std::string to_string() const;
};
```

Represents an IPv4 address.

### Constructor `skyr::ipv4_address::ipv4_address`

``` cpp
ipv4_address();
```

Constructor.

-----

### Constructor `skyr::ipv4_address::ipv4_address`

``` cpp
explicit ipv4_address(unsigned int address);
```

Constructor.

#### Parameters

  - `address` - Sets the IPv4 address to `address`.

-----

### Function `skyr::ipv4_address::address`

``` cpp
unsigned int address() const noexcept;
```

Returns the address value.

*Returns:* The address value.

-----

### Function `skyr::ipv4_address::to_string`

``` cpp
std::string to_string() const;
```

*Returns:* The address as a string.

-----

-----

### Function `skyr::parse_ipv4_address`

``` cpp
expected<skyr::ipv4_address, std::error_code> parse_ipv4_address(std::string_view input);
```

Parses an IPv4 address.

### Parameter `skyr::input`

``` cpp
std::string_view input
```

An input string.

*Returns:* An `ipv4_address` object or an error.

-----

-----
