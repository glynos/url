// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url_serialize.hpp"

namespace skyr {
std::string serialize(
    const url_record &url, bool exclude_fragment) {
  auto output = serialize_excluding_fragment(url);

  if (!exclude_fragment && url.fragment) {
    output += "#";
    output += url.fragment.value();
  }

  return output;
}

std::string serialize_excluding_fragment(
    const url_record &url) {
  auto output = url.scheme + ":";

  if (url.host) {
    output += "//";
    if (url.includes_credentials()) {
      output += url.username;
      if (!url.password.empty()) {
        output += ":";
        output += url.password;
      }
      output += "@";
    }

    // TODO: serialize host
    output += url.host.value();

    if (url.port) {
      output += ":";
      output += std::to_string(url.port.value());
    }
  }
  else if (!url.host && (url.scheme.compare("file") == 0)) {
    output += "//";
  }

  if (url.cannot_be_a_base_url) {
    output += url.path.front();
  }
  else {
    for (const auto &segment : url.path) {
      output += "/";
      output += segment;
    }
  }

  if (url.query) {
    output += "?";
    output += url.query.value();
  }

  return output;
}
}  // namespace skyr
