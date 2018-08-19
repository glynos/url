# Header file `url.hpp`

``` cpp
#define SKYR_URL_INC

namespace skyr
{
    class url_parse_error;

    class url;

    void swap(skyr::url& lhs, skyr::url& rhs) noexcept;

    template <class Source>
    expected<skyr::url, std::error_code> make_url(url::string_type&& input);

    template <class Source>
    expected<skyr::url, std::error_code> make_url(Source const& input);

    template <class Source>
    expected<skyr::url, std::error_code> make_url(url::string_type&& input, skyr::url const& base);

    template <class Source>
    expected<skyr::url, std::error_code> make_url(Source const& input, skyr::url const& base);

    bool operator==(skyr::url const& lhs, skyr::url const& rhs) noexcept;

    bool operator!=(skyr::url const& lhs, skyr::url const& rhs) noexcept;

    bool operator<(skyr::url const& lhs, skyr::url const& rhs) noexcept;

    bool operator>(skyr::url const& lhs, skyr::url const& rhs) noexcept;

    bool operator<=(skyr::url const& lhs, skyr::url const& rhs) noexcept;

    bool operator>=(skyr::url const& lhs, skyr::url const& rhs) noexcept;
}
```

### Class `skyr::url_parse_error`

``` cpp
class url_parse_error
: public std::runtime_error
{
public:
    explicit url_parse_error(std::error_code error) noexcept;

    std::error_code error() const noexcept;
};
```

`url_parse_error` is thrown when there is an error parsing the URL.

### Constructor `skyr::url_parse_error::url_parse_error`

``` cpp
explicit url_parse_error(std::error_code error) noexcept;
```

Constructor

#### Parameters

  - `error` - An error code value.

-----

### Function `skyr::url_parse_error::error`

``` cpp
std::error_code error() const noexcept;
```

*Returns:* An error code.

-----

-----

### Class `skyr::url`

``` cpp
class url
{
public:
    using string_type = std::string;

    using string_view = std::string_view;

    using value_type = string_view::value_type;

    using const_iterator = string_view::const_iterator;

    using iterator = const_iterator;

    using const_reference = string_view::const_reference;

    using reference = const_reference;

    using size_type = string_view::size_type;

    using difference_type = string_view::difference_type;

    url();

    template <class Source>
    explicit url(skyr::url::string_type&& input);

    template <class Source>
    explicit url(Source const& input);

    template <class Source>
    url(Source const& input, skyr::url const& base);

    explicit url(skyr::url_record&& input) noexcept;

    void swap(skyr::url& other) noexcept;

    std::string href() const;

    expected<void, std::error_code> set_href(std::string href);

    std::string to_json() const;

    std::string protocol() const;

    expected<void, std::error_code> set_protocol(std::string protocol);

    std::string username() const;

    expected<void, std::error_code> set_username(std::string username);

    std::string password() const;

    expected<void, std::error_code> set_password(std::string password);

    std::string host() const;

    expected<void, std::error_code> set_host(std::string host);

    std::string hostname() const;

    expected<void, std::error_code> set_hostname(std::string hostname);

    std::string port() const;

    template <typename intT>
    intT port(typename std::is_integral<intT>::type* = nullptr) const;

    expected<void, std::error_code> set_port(std::string port);

    expected<void, std::error_code> set_port(std::uint16_t port);

    std::string pathname() const;

    expected<void, std::error_code> set_pathname(std::string pathname);

    std::string search() const;

    expected<void, std::error_code> set_search(std::string search);

    skyr::url_search_parameters& search_parameters();

    std::string hash() const;

    expected<void, std::error_code> set_hash(std::string hash);

    skyr::url_record record() const;

    bool is_special() const noexcept;

    bool validation_error() const noexcept;

    const_iterator begin() const noexcept;

    const_iterator end() const noexcept;

    string_view view() const noexcept;

    bool empty() const noexcept;

    size_type size() const noexcept;

    size_type length() const noexcept;

    int compare(skyr::url const& other) const noexcept;

    static optional<std::uint16_t> default_port(std::string const& scheme) noexcept;

    void clear();

    char const* c_str() const noexcept;

    operator string_type() const;

    std::string string() const;

    std::wstring wstring() const;

    std::string u8string() const;

    std::u16string u16string() const;

    std::u32string u32string() const;
};
```

Represents a URL. Parsing is performed according to the [WhatWG specification](https://url.spec.whatwg.org/).

### Type alias `skyr::url::string_type`

``` cpp
using string_type = std::string;
```

`std::basic_string<value_type>`

-----

### Type alias `skyr::url::string_view`

``` cpp
using string_view = std::string_view;
```

The internal string\_view.

-----

### Type alias `skyr::url::value_type`

``` cpp
using value_type = string_view::value_type;
```

The character type (ASCII).

-----

### Type alias `skyr::url::const_iterator`

``` cpp
using const_iterator = string_view::const_iterator;
```

A constant iterator with a value type of `value_type`.

-----

### Type alias `skyr::url::iterator`

``` cpp
using iterator = const_iterator;
```

An alias to `const_iterator`

-----

### Type alias `skyr::url::const_reference`

``` cpp
using const_reference = string_view::const_reference;
```

A constant reference with value type of `value_type`.

-----

### Type alias `skyr::url::reference`

``` cpp
using reference = const_reference;
```

An alias to `const_reference`

-----

### Constructor `skyr::url::url`

``` cpp
url();
```

Default constructor.

-----

### Constructor `skyr::url::url`

``` cpp
template <class Source>
explicit url(skyr::url::string_type&& input);
```

Constructor.

### Parameter `skyr::url::input`

``` cpp
skyr::url::string_type&& input
```

The input string.

*Throws:* `url_parse_exception` on parse errors.

-----

-----

### Constructor `skyr::url::url`

``` cpp
template <class Source>
explicit url(Source const& input);
```

Constructor.

### Parameter `skyr::url::input`

``` cpp
Source const& input
```

The input string.

*Throws:* `url_parse_exception` on parse errors.

-----

-----

### Constructor `skyr::url::url`

``` cpp
template <class Source>
url(Source const& input, skyr::url const& base);
```

Constructor.

#### Parameters

  - `input` - The input string.

### Parameter `skyr::url::base`

``` cpp
skyr::url const& base
```

A base URL.

*Throws:* `url_parse_exception` on parse errors.

-----

-----

### Constructor `skyr::url::url`

``` cpp
explicit url(skyr::url_record&& input) noexcept;
```

Constructor.

#### Parameters

  - `input` - A URL record.

-----

### Function `skyr::url::href`

``` cpp
std::string href() const;
```

*Returns:* 

-----

### Function `skyr::url::to_json`

``` cpp
std::string to_json() const;
```

*Returns:* 

-----

### Function `skyr::url::protocol`

``` cpp
std::string protocol() const;
```

*Returns:* 

-----

### Function `skyr::url::username`

``` cpp
std::string username() const;
```

*Returns:* 

-----

### Function `skyr::url::password`

``` cpp
std::string password() const;
```

*Returns:* 

-----

### Function `skyr::url::host`

``` cpp
std::string host() const;
```

*Returns:* 

-----

### Function `skyr::url::hostname`

``` cpp
std::string hostname() const;
```

*Returns:* 

-----

### Function `skyr::url::port`

``` cpp
std::string port() const;
```

*Returns:* 

-----

### Function `skyr::url::port`

``` cpp
template <typename intT>
intT port(typename std::is_integral<intT>::type* = nullptr) const;
```

*Returns:* 

-----

### Function `skyr::url::pathname`

``` cpp
std::string pathname() const;
```

*Returns:* 

-----

### Function `skyr::url::search`

``` cpp
std::string search() const;
```

*Returns:* 

-----

### Function `skyr::url::search_parameters`

``` cpp
skyr::url_search_parameters& search_parameters();
```

*Returns:* 

-----

### Function `skyr::url::hash`

``` cpp
std::string hash() const;
```

*Returns:* 

-----

### Function `skyr::url::record`

``` cpp
skyr::url_record record() const;
```

*Returns:* 

-----

### Function `skyr::url::is_special`

``` cpp
bool is_special() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url::validation_error`

``` cpp
bool validation_error() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url::begin`

``` cpp
const_iterator begin() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url::end`

``` cpp
const_iterator end() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url::view`

``` cpp
string_view view() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url::empty`

``` cpp
bool empty() const noexcept;
```

Tests whether the URL is an empty string.

*Returns:* `true` if the URL is an empty string, `false` otherwise.

-----

### Function `skyr::url::size`

``` cpp
size_type size() const noexcept;
```

*Returns:* The size of the underlying URL string.

*Returns:* The number of bytes in the underlying string.

-----

### Function `skyr::url::length`

``` cpp
size_type length() const noexcept;
```

*Returns:* The length of the URL string.

*Returns:* The number of characters in the underlying string.

-----

### Function `skyr::url::compare`

``` cpp
int compare(skyr::url const& other) const noexcept;
```

Compares this `url` object lexicographically with another.

### Parameter `skyr::url::other`

``` cpp
skyr::url const& other
```

The other `url` object.

*Returns:* `href_.compare(other.href_)`

-----

-----

### Function `skyr::url::clear`

``` cpp
void clear();
```

Clears the underlying URL string.

-----

### Function `skyr::url::c_str`

``` cpp
char const* c_str() const noexcept;
```

Returns the underyling byte buffer.

*Returns:* `href_.c_str()`

-----

### Conversion operator `skyr::url::operator string_type`

``` cpp
operator string_type() const;
```

Returns the underlying string.

*Returns:* `href_`

-----

### Function `skyr::url::string`

``` cpp
std::string string() const;
```

Returns the URL as a `std::string`.

*Returns:* A URL string.

-----

### Function `skyr::url::wstring`

``` cpp
std::wstring wstring() const;
```

Returns the URL as a `std::wstring`.

*Returns:* A URL string.

-----

### Function `skyr::url::u8string`

``` cpp
std::string u8string() const;
```

Returns the URL as a `std::string`.

*Returns:* A URL string.

-----

### Function `skyr::url::u16string`

``` cpp
std::u16string u16string() const;
```

Returns the URL as a `std::u16string`.

*Returns:* A URL string.

-----

### Function `skyr::url::u32string`

``` cpp
std::u32string u32string() const;
```

Returns the URL as a `std::u32string`.

*Returns:* A URL string.

-----

-----

### Function `skyr::swap`

``` cpp
void swap(skyr::url& lhs, skyr::url& rhs) noexcept;
```

Swaps two `url` objects.

Equivalent to `lhs.swap(rhs)`.

#### Parameters

  - `lhs` - The first `url` object.
  - `rhs` - The second `url` object.

-----

### Function `skyr::make_url`

``` cpp
template <class Source>
expected<skyr::url, std::error_code> make_url(url::string_type&& input);
```

Parses a URL string and constructs a `url` object.

### Parameter `skyr::input`

``` cpp
url::string_type&& input
```

The input string.

*Returns:* A `url` object on success, an error on failure.

-----

-----

### Function `skyr::make_url`

``` cpp
template <class Source>
expected<skyr::url, std::error_code> make_url(Source const& input);
```

Parses a URL string and constructs a `url` object.

### Parameter `skyr::input`

``` cpp
Source const& input
```

The input string.

*Returns:* A `url` object on success, an error on failure.

-----

-----

### Function `skyr::make_url`

``` cpp
template <class Source>
expected<skyr::url, std::error_code> make_url(url::string_type&& input, skyr::url const& base);
```

Parses a URL string and constructs a `url` object.

#### Parameters

  - `input` - The input string.

### Parameter `skyr::base`

``` cpp
skyr::url const& base
```

The base URL.

*Returns:* A `url` object on success, an error on failure.

-----

-----

### Function `skyr::make_url`

``` cpp
template <class Source>
expected<skyr::url, std::error_code> make_url(Source const& input, skyr::url const& base);
```

Parses a URL string and constructs a `url` object.

#### Parameters

  - `input` - The input string.

### Parameter `skyr::base`

``` cpp
skyr::url const& base
```

The base URL.

*Returns:* A `url` object on success, an error on failure.

-----

-----

### Function `skyr::operator==`

``` cpp
bool operator==(skyr::url const& lhs, skyr::url const& rhs) noexcept;
```

Tests two URLs for equality according to the [WhatWG specification](https://url.spec.whatwg.org/#url-equivalence).

#### Parameters

  - `lhs` - A `url` object.

### Parameter `skyr::rhs`

``` cpp
skyr::url const& rhs
```

A `url` object.

*Returns:* `true` if the `url` objects are equal, `false` otherwise.

-----

-----

### Function `skyr::operator!=`

``` cpp
bool operator!=(skyr::url const& lhs, skyr::url const& rhs) noexcept;
```

Tests two URLs for inequality.

#### Parameters

  - `lhs` - A `url` object.

### Parameter `skyr::rhs`

``` cpp
skyr::url const& rhs
```

A `url` object.

*Returns:* \!(lhs == rhs)

-----

-----

### Function `skyr::operator<`

``` cpp
bool operator<(skyr::url const& lhs, skyr::url const& rhs) noexcept;
```

Comparison operator

### Parameter `skyr::rhs`

``` cpp
skyr::url const& rhs
```

*Returns:* 

-----

-----

### Function `skyr::operator>`

``` cpp
bool operator>(skyr::url const& lhs, skyr::url const& rhs) noexcept;
```

Comparison operator

### Parameter `skyr::rhs`

``` cpp
skyr::url const& rhs
```

*Returns:* 

-----

-----

### Function `skyr::operator<=`

``` cpp
bool operator<=(skyr::url const& lhs, skyr::url const& rhs) noexcept;
```

Comparison operator

### Parameter `skyr::rhs`

``` cpp
skyr::url const& rhs
```

*Returns:* 

-----

-----

### Function `skyr::operator>=`

``` cpp
bool operator>=(skyr::url const& lhs, skyr::url const& rhs) noexcept;
```

Comparison operator

### Parameter `skyr::rhs`

``` cpp
skyr::url const& rhs
```

*Returns:* 

-----

-----
