# Header file `url_parse_state.hpp`

``` cpp
#define SKYR_URL_PARSE_STATE_INC

namespace skyr
{
    enum class url_parse_state;
}
```

### Enumeration `skyr::url_parse_state`

``` cpp
enum class url_parse_state
{
    scheme_start,
    scheme,
    no_scheme,
    special_relative_or_authority,
    path_or_authority,
    relative,
    relative_slash,
    special_authority_slashes,
    special_authority_ignore_slashes,
    authority,
    host,
    hostname,
    port,
    file,
    file_slash,
    file_host,
    path_start,
    path,
    cannot_be_a_base_url_path,
    query,
    fragment
};
```

States of the URL parser.

#### Enumerators

  - `scheme_start` - Pointer is at the scheme start.
  - `scheme` - Pointer is at the scheme part.
  - `no_scheme` - For URLs that have no scheme (this may still be valid).
  - `special_relative_or_authority` - URL may be either relative or authority.
  - `path_or_authority` - URL may be either path or authority.
  - `relative` - URL is relative and pointer is at the relative part.
  - `relative_slash` - URL is relative and pointer is at the relative part and its value is a slash.
  - `special_authority_slashes` - URL is an authority and pointer is currently at two slashes.
  - `special_authority_ignore_slashes` - Pointer is at the authority part and can ignore slashes.
  - `authority` - Pointer is at the authority part.
  - `host` - Pointer is at the host part.
  - `hostname` - Pointer is at the hostname part.
  - `port` - Pointer is at the port part.
  - `file` - URL is a file.
  - `file_slash` - URL is a file and pointer value is a slash.
  - `file_host` - URL is a file and pointer is at the host part.
  - `path_start` - Pointer is at the start of the path part.
  - `path` - Pointer is at the path part.
  - `cannot_be_a_base_url_path` - URL cannot be a base URL.
  - `query` - Pointer is at the query part.
  - `fragment` - Pointer is at the fragment part.

-----
