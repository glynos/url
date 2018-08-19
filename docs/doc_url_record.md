# Header file `url_record.hpp`

``` cpp
#define SKYR_URL_RECORD_INC

namespace skyr
{
    struct url_record;

    void swap(skyr::url_record& lhs, skyr::url_record& rhs) noexcept;
}
```

### Struct `skyr::url_record`

``` cpp
struct url_record
{
    using string_type = std::string;

    string_type scheme;

    string_type username;

    string_type password;

    optional<skyr::url_record::string_type> host;

    optional<std::uint16_t> port;

    std::vector<string_type> path;

    optional<skyr::url_record::string_type> query;

    optional<skyr::url_record::string_type> fragment;

    bool cannot_be_a_base_url;

    url_record();

    bool is_special() const noexcept;

    bool includes_credentials() const noexcept;

    bool cannot_have_a_username_password_or_port() const noexcept;

    void swap(skyr::url_record& other) noexcept;
};
```

Represents the parts of a URL identifier.

#### Member variables

  - `scheme` - An ASCII string that identifies the type of URL.
  - `username` - An ASCII string identifying a username.
  - `password` - An ASCII string identifying a password.
  - `host` - An optional URL host, either a domain, IPv4 or IPv6 address, an opaque host, or empty.
  - `port` - An optional network port.
  - `path` - A list of zero or more ASCII strings, used to identify a location in a hierarchical form.
  - `query` - An optional ASCII string.
  - `fragment` - An optional ASCII string.
  - `cannot_be_a_base_url` - A Boolean value indicating whether this URL can be used as a base URL.

### Constructor `skyr::url_record::url_record`

``` cpp
url_record();
```

Default constructor.

-----

### Function `skyr::url_record::is_special`

``` cpp
bool is_special() const noexcept;
```

Tests if the URL is a special scheme.

*Returns:* `true` if the URL scheme is a special scheme, `false` otherwise.

Equivalent to …

-----

### Function `skyr::url_record::includes_credentials`

``` cpp
bool includes_credentials() const noexcept;
```

Tests if the URL includes credentials.

*Returns:* `true` if the URL username or password is not an empty string, `false` otherwise.

Equivalent to

-----

### Function `skyr::url_record::cannot_have_a_username_password_or_port`

``` cpp
bool cannot_have_a_username_password_or_port() const noexcept;
```

Tests if the URL cannot have a username, password or port.

*Returns:* `true` if the URL cannot have a username, password or port.

Equivalent to …

-----

### Function `skyr::url_record::swap`

``` cpp
void swap(skyr::url_record& other) noexcept;
```

Swaps two `url_record` objects.

#### Parameters

  - `other` - Another `url_record` object.

-----

-----

### Function `skyr::swap`

``` cpp
void swap(skyr::url_record& lhs, skyr::url_record& rhs) noexcept;
```

Swaps to `url_record` objects.

Equivalent to `lhs.swap(rhs)`

#### Parameters

  - `lhs` - A `url_record` object.
  - `rhs` - A `url_record` object.

-----
