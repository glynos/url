# Header file `optional.hpp`

``` cpp
#define SKYR_OPTIONAL_INC

namespace skyr
{
    template <class T>
    using optional = tl::optional<T>;

    using bad_optional_access = tl::bad_optional_access;

    constexpr auto const nullopt = tl::nullopt;
}
```
