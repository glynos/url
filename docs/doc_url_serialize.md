# Header file `url_serialize.hpp`

``` cpp
#define SKYR_URL_SERIALIZE_INC

namespace skyr
{
    std::string serialize(skyr::url_record const& url, bool exclude_fragment = false);
}
```

### Function `skyr::serialize`

``` cpp
std::string serialize(skyr::url_record const& url, bool exclude_fragment = false);
```

Serializes a URL record according to the [WhatWG specification](https://url.spec.whatwg.org/#url-serializing).

### Parameter `skyr::url`

``` cpp
skyr::url_record const& url
```

A URL record

*Returns:* A serialized URL string

-----

-----
