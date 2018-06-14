// Copyright 2016-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_OPTIONAL_INC
#define SKYR_OPTIONAL_INC

#include <tl/optional.hpp>

namespace skyr {
template <class T>
using optional = tl::optional<T>;
using bad_optional_access = tl::bad_optional_access;

constexpr auto nullopt = tl::nullopt;
}  // namespace skyr

#endif  // SKYR_OPTIONAL_INC
