# Header file `expected.hpp`

``` cpp
#define SKYR_EXPECTED_INC

namespace skyr
{
    template <class T, class E>
    using expected = tl::expected<T, E>;

    template <class E>
    using bad_expected_access = tl::bad_expected_access<E>;

    template <class E>
    using unexpected = tl::unexpected<E>;

    template <class E>
    unexpected<E> make_unexpected(E&& e);
}
```
