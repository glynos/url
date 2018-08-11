// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url_record.hpp"
#include "url_schemes.hpp"

namespace skyr {
bool url_record::is_special() const noexcept {
  return skyr::details::is_special(string_view(scheme));
}

bool url_record::includes_credentials() const noexcept {
  return !username.empty() || !password.empty();
}

bool url_record::cannot_have_a_username_password_or_port() const noexcept {
  return (!host || host.value().empty()) || cannot_be_a_base_url || (scheme == "file");
}
}  // namespace skyr
