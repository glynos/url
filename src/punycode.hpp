// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_PUNYCODE_HPP
#define SKYR_PUNYCODE_HPP

#include <string>
#include <skyr/string_view.hpp>
#include <skyr/expected.hpp>

namespace skyr {
/// Punycode functions
namespace punycode {
/// Punycode encoding or decoding error status
enum class punycode_errc {
  fail,
};

/// \param input
/// \returns
expected<std::string, punycode_errc> encode(string_view input);

/// \param input
/// \returns
expected<std::string, punycode_errc> decode(string_view input);
}  // namespace punycode
}  // namespace skyr

#endif //SKYR_PUNYCODE_HPP
