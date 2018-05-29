// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url/url_search_parameters.hpp"
#include <algorithm>

namespace skyr {
url_search_parameters::url_search_parameters() { update(); }

url_search_parameters::url_search_parameters(string_view query) {
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
  
url_search_parameters::url_search_parameters(const string_type &query)
  : url_search_parameters(string_view(query)) {}

url_search_parameters::url_search_parameters(std::initializer_list<value_type> parameters)
  : parameters_(parameters) {}

void url_search_parameters::append(const string_type &name,
                              const string_type &value) {
  parameters_.emplace_back(name, value);
  update();
}

void url_search_parameters::remove(const string_type &name) {
  auto it = std::remove_if(std::begin(parameters_), std::end(parameters_),
                           [&name](const value_type &parameter) -> bool {
                             return name.compare(parameter.first) == 0;
                           });
  parameters_.erase(it, std::end(parameters_));
  update();
}

optional<url_search_parameters::string_type> url_search_parameters::get(
    const string_type &name) const noexcept {
  auto it = std::find_if(std::begin(parameters_), std::end(parameters_),
                         [&name](const value_type &parameter) -> bool {
                           return name.compare(parameter.first) == 0;
                         });
  if (it == std::end(*this)) {
    return nullopt;
  }

  return it->second;
}

std::vector<url_search_parameters::string_type> url_search_parameters::get_all(
    const string_type &name) const {
  std::vector<string_type> result;
  for (const auto &param : parameters_) {
    if (param.first == name) {
      result.push_back(param.second);
    }
  }
  return result;
}

bool url_search_parameters::contains(const string_type &name) const noexcept {
  return std::end(*this) !=
         std::find_if(std::begin(parameters_), std::end(parameters_),
                      [&name](const value_type &parameter) -> bool {
                        return name.compare(parameter.first) == 0;
                      });
}

void url_search_parameters::set(const string_type &name, const string_type &value) {
  auto it = std::find_if(std::begin(parameters_), std::end(parameters_),
                         [&name](const value_type &parameter) -> bool {
                           return name.compare(parameter.first) == 0;
                         });
  if (it != std::end(*this)) {
    it->second = value;
  }
  else {
    parameters_.emplace_back(name, value);
  }

  update();
}

url_search_parameters::const_iterator url_search_parameters::begin() const noexcept {
  return parameters_.begin();
}

url_search_parameters::const_iterator url_search_parameters::end() const noexcept {
  return parameters_.end();
}

url_search_parameters::string_type url_search_parameters::to_string() const {
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

void url_search_parameters::sort() {
  std::sort(std::begin(parameters_), std::end(parameters_),
            [](const value_type &lhs, const value_type &rhs) -> bool {
              return lhs.first < rhs.first;
            });

  update();
}

void url_search_parameters::update() {}
}  // namespace skyr
