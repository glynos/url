// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef SKYR_IDNA_TABLE_HPP
#define SKYR_IDNA_TABLE_HPP

namespace skyr {
enum class idna_status {
  disallowed = 1,
  disallowed_std3_valid,
  disallowed_std3_mapped,
  ignored,
  mapped,
  deviation,
  valid,
};

idna_status map_idna_status(char32_t c);

char32_t map_idna_char(char32_t c);
}  // namespace skyr

#endif //SKYR_IDNA_TABLE_HPP
