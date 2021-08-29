// Copyright 2019-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <type_traits>

export module skyr.v3.unicode.traits.range_iterator;

export {
  namespace skyr::inline v3::unicode::traits {
  ///
  /// \tparam Range
  template <class Range>
  struct range_iterator {
    using type = typename std::decay_t<Range>::const_iterator;
  };

  ///
  /// \tparam T
  /// \tparam N
  template <typename T, std::size_t N>
  struct range_iterator<T[N]> {  // NOLINT
    using type = const T *;
  };

  template <class Range>
  using range_iterator_t = typename range_iterator<Range>::type;
  }  // namespace skyr::inline v3::unicode::traits
}
