// Copyright 2012-2018 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <locale>
#include <algorithm>
#include <functional>
#include <vector>
#include "skyr/url/url.hpp"
#include "skyr/url/url_parse.hpp"
#include "detail/uri_advance_parts.hpp"
#include "detail/uri_percent_encode.hpp"
#include "detail/algorithm.hpp"
#include "detail/url_schemes.hpp"

namespace skyr {
  url::url(const std::string &input) {
    auto parsed_url = parse(input);
    if (!parsed_url.success) {
      throw type_error();
    }

    url_ = parsed_url;

    auto query = parsed_url.query? parsed_url.query.value() : std::string();
    // TODO: set query object
  }

  url::url(const std::string &input, const std::string &base) {
    auto parsed_base = parse(base);
    if (!parsed_base.success) {
      throw type_error();
    }

    auto parsed_url = parse(input, parsed_base);
    if (!parsed_url.success) {
      throw type_error();
    }

    url_ = parsed_url;

    auto query = parsed_url.query? parsed_url.query.value() : std::string();
    // TODO: set query object
  }

  std::string url::href() const {
    return std::string();
  }

  std::string url::origin() const {
    return std::string();
  }

  std::string url::protocol() const {
    return url_.scheme + ":";
  }

  std::string url::username() const {
    return url_.username;
  }

  std::string url::password() const {
    return url_.password;
  }

  std::string url::host() const {
    if (!url_.host) {
      return std::string();
    }

    if (!url_.port) {
      return url_.host.value();
    }

    return url_.host.value() + ":" + std::to_string(url_.port.value());
  }

  std::string url::hostname() const {
    if (!url_.host) {
      return std::string();
    }

    return url_.host.value();
  }

  std::string url::port() const {
    if (!url_.port) {
      return std::string();
    }

    return std::to_string(url_.port.value());
  }

  std::string url::pathname() const {
    if (url_.cannot_be_a_base_url) {
      return url_.path.front();
    }

    if (url_.path.empty()) {
      return std::string("");
    }

    auto pathname = std::string("");
  //  std::cout << "!!! " << url_.path[0] << "|||" << std::endl;
  //  if (!url_.path[0].empty()) {
  //    pathname += "/";
  //  }

    for (const auto &segment : url_.path) {
      pathname += segment;
      pathname += "/";
    }
    pathname = pathname.substr(0, pathname.length() - 1);

  //  auto view = string_view(pathname);
  //  if (remaining_starts_with(begin(view), end(view), "//")) {
  //    pathname = pathname.substr(1, std::string::npos);
  //  }

    return pathname;
  }

  std::string url::search() const {
    if (!url_.query || url_.query.value().empty()) {
      return std::string();
    }

    return "?" + url_.query.value();
  }

  std::string url::hash() const {
    if (!url_.fragment || url_.fragment.value().empty()) {
      return std::string();
    }

    return "#" + url_.fragment.value();
  }
}