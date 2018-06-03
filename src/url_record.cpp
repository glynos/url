// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url_parse.hpp"
#include "skyr/url_record.hpp"
#include "url_schemes.hpp"

namespace skyr {
bool url_record::is_special() const {
  return skyr::details::is_special(basic_string_view<char>(scheme));
}

bool url_record::includes_credentials() const {
  return !username.empty() || !password.empty();
}
}  // namespace skyr
