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
#include "network/uri/whatwg/url.hpp"
#include "detail/uri_parse.hpp"
#include "detail/uri_advance_parts.hpp"
#include "detail/uri_percent_encode.hpp"
#include "detail/algorithm.hpp"
#include "detail/url_schemes.hpp"

namespace network {
namespace whatwg {
url::url() : url_view_(url_), url_parts_(), cannot_be_a_base_url_(false) {}

url::url(const url &other)
    : url_(other.url_),
      url_view_(url_),
      url_parts_(),
      cannot_be_a_base_url_(other.cannot_be_a_base_url_) {
  ::network::detail::advance_parts(url_view_, url_parts_, other.url_parts_);
}

url::url(url &&other) noexcept
    : url_(std::move(other.url_)),
      url_view_(url_),
      url_parts_(std::move(other.url_parts_)),
      cannot_be_a_base_url_(std::move(other.cannot_be_a_base_url_)) {
  ::network::detail::advance_parts(url_view_, url_parts_, other.url_parts_);
  other.url_.clear();
  other.url_view_ = string_view(other.url_);
  other.url_parts_ = ::network::detail::uri_parts();
}

url::~url() {}

url &url::operator=(url other) {
  other.swap(*this);
  return *this;
}

void url::swap(url &other) noexcept {
  auto parts = url_parts_;
  advance_parts(other.url_view_, url_parts_, other.url_parts_);
  url_.swap(other.url_);
  url_view_.swap(other.url_view_);
  advance_parts(other.url_view_, other.url_parts_, parts);
  std::swap(cannot_be_a_base_url_, other.cannot_be_a_base_url_);
}

url::const_iterator url::begin() const noexcept { return url_view_.begin(); }

url::const_iterator url::end() const noexcept { return url_view_.end(); }

bool url::has_scheme() const noexcept {
  return static_cast<bool>(url_parts_.scheme);
}

url::string_view url::scheme() const noexcept {
  return has_scheme() ? static_cast<string_view>(*url_parts_.scheme)
                      : string_view{};
}

bool url::has_user_info() const noexcept {
  return static_cast<bool>(url_parts_.user_info);
}

url::string_view url::user_info() const noexcept {
  return has_user_info() ? static_cast<string_view>(*url_parts_.user_info)
                         : string_view{};
}

bool url::has_host() const noexcept {
  return static_cast<bool>(url_parts_.host);
}

url::string_view url::host() const noexcept {
  return has_host() ? static_cast<string_view>(*url_parts_.host)
                    : string_view{};
}

bool url::has_port() const noexcept {
  return static_cast<bool>(url_parts_.port);
}

url::string_view url::port() const noexcept {
  return has_port() ? static_cast<string_view>(*url_parts_.port)
                    : string_view{};
}

bool url::has_path() const noexcept {
  return static_cast<bool>(url_parts_.path);
}

url::string_view url::path() const noexcept {
  return has_path() ? static_cast<string_view>(*url_parts_.path)
                    : string_view{};
}

url::path_iterator url::path_begin() const noexcept {
  return has_path()? url::path_iterator{url_parts_.path} : url::path_iterator{};
}

url::path_iterator url::path_end() const noexcept {
  return url::path_iterator{};
}

bool url::has_query() const noexcept {
  return static_cast<bool>(url_parts_.query);
}

url::string_view url::query() const noexcept {
  return has_query() ? static_cast<string_view>(*url_parts_.query)
                     : string_view{};
}

url::query_iterator url::query_begin() const noexcept {
  return has_query()? url::query_iterator{url_parts_.query} : url::query_iterator{};
}

url::query_iterator url::query_end() const noexcept {
  return url::query_iterator{};
}

bool url::has_fragment() const noexcept {
  return static_cast<bool>(url_parts_.fragment);
}

url::string_view url::fragment() const noexcept {
  return has_fragment() ? static_cast<string_view>(*url_parts_.fragment)
                        : string_view{};
}

std::string url::string() const { return url_; }

std::wstring url::wstring() const {
  return std::wstring(std::begin(*this), std::end(*this));
}

std::u16string url::u16string() const {
  return std::u16string(std::begin(*this), std::end(*this));
}

std::u32string url::u32string() const {
  return std::u32string(std::begin(*this), std::end(*this));
}

bool url::empty() const noexcept { return url_.empty(); }

bool url::is_absolute() const noexcept { return has_scheme(); }

bool url::is_opaque() const noexcept {
  return (is_absolute() && !has_host());
}

bool url::is_special() const noexcept {
  if (has_scheme()) {
    return detail::is_special(scheme().substr(0, scheme().length() - 1));
  }
  return false;
}

optional<std::uint16_t> url::default_port(const string_type &scheme) {
  return detail::default_port(string_view(scheme));
}

namespace {
url::string_type serialize_host(url::string_view host) {
  return url::string_type{host};
}

url::string_type serialize_port(url::string_view port) {
  return url::string_type{port};
}
}  // namespace

url url::serialize() const {
  // https://url.spec.whatwg.org/#url-serializing
  auto result = string_type(scheme());

  if (has_host()) {
    result += "//";
    if (has_user_info()) {
      result += string_type(user_info());
      result += "@";
    }

    result += serialize_host(host());

    if (has_port()) {
      result += ":";
      result += serialize_port(port());
    }
  }
  else if (scheme().compare("file:") == 0) {
    result += "//";
  }

  auto path_it = path_begin();
  if (cannot_be_a_base_url_) {
    result += string_type(*path_it);
  }
  else {
    result += string_type(path());
  }

  if (has_query()) {
    result += "?";
    result += string_type(query());
  }

  if (has_fragment()) {
    result += "#";
    result += string_type(fragment());
  }

  return url{result};
}

int url::compare(const url &other) const {
  // if both URLs are empty, then we should define them as equal
  // even though they're still invalid.
  if (empty() && other.empty()) {
    return 0;
  }

  if (empty()) {
    return -1;
  }

  if (other.empty()) {
    return 1;
  }

  return serialize().url_.compare(other.serialize().url_);
}

bool url::initialize(const string_type &url) {
  url_ = ::network::detail::trim_copy(url);

//  auto it = std::remove_if(std::begin(url_), std::end(url_),
//                           [] (char v) -> bool { return (v == '\t') || (v == '\n') || (v == '\r'); });
//  url_.erase(std::begin(url_), it);

  if (!url_.empty()) {
    url_view_ = string_view(url_);
    const_iterator it = std::begin(url_view_), last = std::end(url_view_);
    bool is_valid = ::network::detail::parse(it, last, url_parts_);
    return is_valid;
  }
  return true;
}

void swap(url &lhs, url &rhs) noexcept { lhs.swap(rhs); }

bool operator==(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

bool operator==(const url &lhs, const char *rhs) noexcept {
  if (std::strlen(rhs) !=
      std::size_t(std::distance(std::begin(lhs), std::end(lhs)))) {
    return false;
  }
  return std::equal(std::begin(lhs), std::end(lhs), rhs);
}

bool operator<(const url &lhs, const url &rhs) noexcept {
  return lhs.compare(rhs) < 0;
}
}  // namespace whatwg
}  // namespace network
