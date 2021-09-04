// Copyright 2020-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <locale>
#include <string>
#include <algorithm>
#include <iterator>

export module skyr.v3.core.check_input;

export {
  namespace skyr::inline v3 {
  constexpr static auto is_c0_control_or_space = [](auto byte) {
    return std::iscntrl(byte, std::locale::classic()) || std::isspace(byte, std::locale::classic());
  };

  constexpr inline auto remove_leading_c0_control_or_space(std::string_view input, bool *validation_error) {
    auto first = std::cbegin(input), last = std::cend(input);
    auto it = std::find_if_not(first, last, is_c0_control_or_space);
    *validation_error |= (it != first);
    input.remove_prefix(std::distance(first, it));
    return input;
  }

  constexpr inline auto remove_trailing_c0_control_or_space(std::string_view input, bool *validation_error) {
    auto first = std::crbegin(input), last = std::crend(input);
    auto it = std::find_if_not(first, last, is_c0_control_or_space);
    *validation_error |= (it != first);
    input.remove_suffix(std::distance(first, it));
    return input;
  }
  }  // namespace skyr::inline v3
}
