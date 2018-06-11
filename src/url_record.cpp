// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url_parse.hpp"
#include "skyr/url_record.hpp"
#include "url_schemes.hpp"
#include "skyr/ipv4_address.hpp"
#include <sstream>
#include <cmath>

namespace skyr {

std::string ipv4_address::to_string() const {
  auto output = std::string();

  auto n = repr;

  for (auto i = 1U; i <= 4U; ++i) {
    std::ostringstream oss;
    oss << (n % 256);
    output = oss.str() + output;

    if (i != 4) {
      output = "." + output;
    }

    n = static_cast<std::uint32_t>(std::floor(n / 256.));
  }

  return output;
}

bool url_record::is_special() const {
  return skyr::details::is_special(basic_string_view<char>(scheme));
}

bool url_record::includes_credentials() const {
  return !username.empty() || !password.empty();
}
}  // namespace skyr
