// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_EXPECTED_INC
#define SKYR_EXPECTED_INC

#include <tl/expected.hpp>

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
namespace skyr {
template <class T, class E>
using expected = tl::expected<T, E>;
template <class E>
using bad_expected_access = tl::bad_expected_access<E>;
template <class E>
using unexpected = tl::unexpected<E>;

template <class E>
inline unexpected<E> make_unexpected(E &&e) {
  return tl::make_unexpected(e);
}
}  // namespace skyr
#else
namespace skyr {
/// An `expected<T, E>` object is an object that contains the storage for
/// another object and manages the lifetime of this contained object `T`.
/// Alternatively it could contain the storage for another unexpected object
/// `E`. The contained object may not be initialized after the expected object
/// has been initialized, and may not be destroyed before the expected object
/// has been destroyed. The initialization state of the contained object is
/// tracked by the expected object.
///
/// The full documentation for this implementation can be found
/// [here](https://expected.tartanllama.xyz/).
template <class T, class E>
class expected;

/// Thrown when accessing an expected object that contains an
/// unexpected value
class bad_expected_access {
 public:
};
}  // namespace skyr
#endif  // !defined(DOXYGEN_SHOULD_SKIP_THIS)

#endif  // SKYR_OPTIONAL_INC
