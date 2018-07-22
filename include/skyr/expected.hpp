// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_EXPECTED_INC
#define SKYR_EXPECTED_INC

#include <tl/expected.hpp>

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

#endif  // SKYR_OPTIONAL_INC
