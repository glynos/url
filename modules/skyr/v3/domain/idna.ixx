// Copyright 2018-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <iterator>

export module skyr.v3.domain.idna;

export import skyr.v3.domain.idna_status;
import skyr.v3.domain.idna_tables;

export {
  namespace skyr::inline v3::idna {
  
    ///
    /// \param code_point A code point value
    /// \return The status of the code point
    constexpr auto code_point_status(char32_t code_point) -> idna_status {
      constexpr auto less = [](const auto &range, auto code_point) { return range.last < code_point; };

      auto first = std::cbegin(statuses), last = std::cend(statuses);
      auto it = std::lower_bound(first, last, code_point, less);
      return (it == last) || !((code_point >= (*it).first) && (code_point <= (*it).last)) ? idna_status::valid
                                                                                          : it->status;
    }
  }  // namespace skyr::inline v3::idna
} // export
