// Copyright 2016-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_OPTIONAL_INC
#define SKYR_OPTIONAL_INC

#include <skyr/external/tl/optional.hpp>

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
namespace skyr {
template <class T>
using optional = tl::optional<T>;
using bad_optional_access = tl::bad_optional_access;

constexpr auto nullopt = tl::nullopt;
}  // namespace skyr
#else
namespace skyr {
/// \class optional optional.hpp <skyr/optional.hpp>
///
/// An optional object is an object that contains the storage for another
/// object and manages the lifetime of this contained object, if any. The
/// contained object may be initialized after the optional object has been
/// initialized, and may be destroyed before the optional object has been
/// destroyed. The initialization state of the contained object is tracked by
/// the optional object.
///
/// The full documentation for this implementation can be found
/// [here](https://optional.tartanllama.xyz/).
template <class T>
class optional;

/// Thrown when accessing an optional object that does not contain
/// a value
class bad_optional_access {
 public:
};
}  // namespace skyr
#endif  // !defined(DOXYGEN_SHOULD_SKIP_THIS)

#endif  // SKYR_OPTIONAL_INC
