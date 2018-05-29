// Copyright 2013-2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef URI_TEST_STRING_UTILITY_INC
#define URI_TEST_STRING_UTILITY_INC

#include <skyr/string_view.hpp>

namespace skyr {
inline bool operator==(string_view lhs, const std::string &rhs) {
  return lhs.compare(rhs.c_str()) == 0;
}

inline bool operator==(const std::string &lhs, string_view rhs) {
  return rhs == lhs;
}
}  // namespace skyr

#endif  // URI_TEST_STRING_UTILITY_INC
