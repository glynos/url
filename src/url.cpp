// Copyright 2012-2018 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cassert>
#include <functional>
#include <locale>
#include <vector>
#include "skyr/url.hpp"
#include "skyr/url_parse.hpp"
#include "skyr/url_serialize.hpp"
#include "url_schemes.hpp"

namespace skyr {
url::url(std::string input) {
  auto parsed_url = parse(input);
  if (!parsed_url) {
    throw type_error();
  }

  url_ = parsed_url.value();
  view_ = string_view(url_.url);

  // auto query = url_.query ? url_.query.value() : std::string();
  // TODO: set query object
}

url::url(std::string input, std::string base) {
  auto parsed_base = parse(base);
  if (!parsed_base) {
    throw type_error();
  }

  auto parsed_url = parse(input, parsed_base.value());
  if (!parsed_url) {
    throw type_error();
  }

  url_ = parsed_url.value();
  view_ = string_view(url_.url);

  // auto query = url_.query ? url_.query.value() : std::string();
  // TODO: set query object
}

url::url(url_record &&input) noexcept
  : url_(input)
  , view_(url_.url) {}


std::string url::href() const { return std::string(); }

std::string url::origin() const { return std::string(); }

std::string url::protocol() const { return url_.scheme + ":"; }

std::string url::username() const { return url_.username; }

std::string url::password() const { return url_.password; }

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

  if ((url_.path.size() == 1) && url_.path[0].empty()) {
    return std::string("/");
  }

  auto pathname = std::string("");
  if (!url_.path[0].empty()) {
    pathname += "/";
  }

  for (const auto &segment : url_.path) {
    pathname += segment;
    pathname += "/";
  }
  pathname = pathname.substr(0, pathname.length() - 1);

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

bool url::is_special() const noexcept { return url_.is_special(); }

bool url::validation_error() const noexcept { return false; }

std::string url::serialize() const {
  using skyr::serialize;
  return serialize(url_, true);
}

optional<std::uint16_t> url::default_port(const std::string &scheme) noexcept {
  return details::default_port(string_view(scheme));
}

expected<url, parse_error> make_url(std::string input) noexcept {
  auto parsed_url = parse(input);
  if (!parsed_url) {
    return make_unexpected(std::move(parsed_url.error()));
  }

  return url(std::move(parsed_url.value()));
}

expected<url, parse_error> make_url(std::string input, std::string base) noexcept {
  auto parsed_base = parse(base);
  if (!parsed_base) {
    return make_unexpected(std::move(parsed_base.error()));
  }

  auto parsed_url = parse(input, parsed_base.value());
  if (!parsed_url) {
    return make_unexpected(std::move(parsed_url.error()));
  }

  return url(std::move(parsed_url.value()));
}
}  // namespace skyr