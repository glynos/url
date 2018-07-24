// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_ERROR_INC
#define SKYR_URL_ERROR_INC

#include <string>
#include <stdexcept>
#include <skyr/url_parse_state.hpp>

namespace skyr {
///
struct parse_error {
  ///
  url_parse_state state;
  ///
  std::string parse_until;
};

/// This exception is used when there is an error parsing the URL.
class type_error : public std::runtime_error {
 public:
   /// Constructor
  type_error() : runtime_error("Type error") {}

};
}  // namespace skyr

#endif // SKYR_URL_ERROR_INC
