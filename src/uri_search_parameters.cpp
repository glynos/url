// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "network/uri/whatwg/uri_search_parameters.hpp"

namespace network {
namespace whatwg {
uri_search_parameters::uri_search_parameters() { update(); }

uri_search_parameters::uri_search_parameters(const string_type &query) {
  auto first = std::begin(query), last = std::end(query);

  for (auto it = first; it != last;) {
    auto sep_it = std::find_if(
        it, last, [](char c) -> bool { return c == '&' || c == ';'; });
    auto eq_it =
        std::find_if(it, sep_it, [](char c) -> bool { return c == '='; });

    auto name = string_type(it, eq_it);
    if (eq_it != sep_it) {
      ++eq_it;  // skip '=' symbol
    }
    auto value = string_type(eq_it, sep_it);

    parameters_.emplace_back(name, value);

    it = sep_it;
    if (*it == '&' || *it == ';') {
      ++it;
    }
  }

  update();
}

uri_search_parameters::uri_search_parameters(std::initializer_list<value_type> parameters)
    : parameters_(parameters) {}

void uri_search_parameters::append(const string_type &name,
                              const string_type &value) {
  parameters_.emplace_back(name, value);
  update();
}

void uri_search_parameters::remove(const string_type &name) {
  auto it = std::remove_if(std::begin(parameters_), std::end(parameters_),
                           [&name](const value_type &parameter) -> bool {
                             return parameter.first == name;
                           });
  parameters_.erase(it, std::end(parameters_));

  update();
}

optional<uri_search_parameters::string_type> uri_search_parameters::get(
    const string_type &name) const noexcept {
  auto it = std::find_if(std::begin(*this), std::end(*this),
                         [&name](const value_type &parameter) -> bool {
                           return parameter.first == name;
                         });
  if (it == std::end(*this)) {
    return nullopt;
  }

  return it->second;
}

bool uri_search_parameters::contains(const string_type &name) const noexcept {
  return std::end(*this) !=
         std::find_if(std::begin(*this), std::end(*this),
                      [&name](const value_type &parameter) -> bool {
                        return parameter.first == name;
                      });
}

void uri_search_parameters::set(const string_type &name, const string_type &value) {
  if (contains(name)) {
    for (auto &parameter : parameters_) {
      if (parameter.first == name) {
        parameter.second = value;
      }
    }
  } else {
    parameters_.emplace_back(name, value);
  }

  update();
}

uri_search_parameters::const_iterator uri_search_parameters::begin() const noexcept {
  return parameters_.begin();
}

uri_search_parameters::const_iterator uri_search_parameters::end() const noexcept {
  return parameters_.end();
}

uri_search_parameters::string_type uri_search_parameters::to_string() const {
  auto result = string_type{};

  auto first = std::begin(parameters_), last = std::end(parameters_);
  auto it = first;
  while (it != last) {
    result.append(it->first);
    result.append("=");
    result.append(it->second);

    ++it;

    if (it != last) {
      result.append("&");
    }
  }

  return result;
}

void uri_search_parameters::sort() {
  std::sort(std::begin(parameters_), std::end(parameters_),
            [](const value_type &lhs, const value_type &rhs) -> bool {
              return lhs.first < rhs.first;
            });

  update();
}

void uri_search_parameters::update() {}
}  // namespace whatwg
}  // namespace network
