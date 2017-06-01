// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "network/uri/uri_query_parameters.hpp"

namespace network {
query_parameters::query_parameters() { update(); }

query_parameters::query_parameters(const string_type &query) {
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

query_parameters::query_parameters(std::initializer_list<value_type> parameters)
    : parameters_(parameters) {}

void query_parameters::append(const string_type &name,
                              const string_type &value) {
  parameters_.emplace_back(name, value);
  update();
}

void query_parameters::remove(const string_type &name) {
  auto it = std::remove_if(std::begin(parameters_), std::end(parameters_),
                           [&name](const value_type &parameter) -> bool {
                             return parameter.first == name;
                           });
  parameters_.erase(it, std::end(parameters_));

  update();
}

optional<query_parameters::string_type> query_parameters::get(
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

bool query_parameters::contains(const string_type &name) const noexcept {
  return std::end(*this) !=
         std::find_if(std::begin(*this), std::end(*this),
                      [&name](const value_type &parameter) -> bool {
                        return parameter.first == name;
                      });
}

void query_parameters::set(const string_type &name, const string_type &value) {
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

query_parameters::const_iterator query_parameters::begin() const noexcept {
  return parameters_.begin();
}

query_parameters::const_iterator query_parameters::end() const noexcept {
  return parameters_.end();
}

query_parameters::string_type query_parameters::to_string() const {
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

void query_parameters::sort() {
  std::sort(std::begin(parameters_), std::end(parameters_),
            [](const value_type &lhs, const value_type &rhs) -> bool {
              return lhs.first < rhs.first;
            });

  update();
}

void query_parameters::update() {}
}  // namespace network
