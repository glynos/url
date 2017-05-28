// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARTS_INC
#define NETWORK_DETAIL_URI_PARTS_INC

#include <string>
#include <utility>
#include <iterator>
#include <network/optional.hpp>
#include <network/string_view.hpp>

namespace network {
namespace detail {
class uri_part {
 public:
  using value_type = string_view::value_type;
  using iterator = string_view::iterator;
  using const_iterator = string_view::const_iterator;
  using const_pointer = string_view::const_pointer;
  using size_type = string_view::size_type;
  using difference_type = string_view::difference_type;

  uri_part() noexcept = default;

  uri_part(const_iterator first, const_iterator last) noexcept
    : view_(std::addressof(*first), std::distance(first, last)) {}

  explicit uri_part(string_view view) noexcept
    : view_(view) {}

  const_iterator begin() const noexcept { return view_.begin(); }

  const_iterator end() const noexcept { return view_.end(); }

  bool empty() const noexcept { return view_.empty(); }

  string_view to_string_view() const noexcept { return view_; }

  std::string to_string() const { return view_.to_string(); }

 private:
  string_view view_;
};

struct hierarchical_part {
  hierarchical_part() = default;

  optional<uri_part> user_info;
  optional<uri_part> host;
  optional<uri_part> port;
  optional<uri_part> path;
};

struct uri_parts {
  uri_parts() = default;

  optional<uri_part> scheme;
  hierarchical_part hier_part;
  optional<uri_part> query;
  optional<uri_part> fragment;
};
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_URI_PARTS_INC
