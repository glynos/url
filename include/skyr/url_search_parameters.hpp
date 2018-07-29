// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_URL_SEARCH_PARAMETERS_INC
#define SKYR_URL_SEARCH_PARAMETERS_INC

#include <string>
#include <vector>
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
///
class url_search_parameters {
 public:

  using string_type = std::string;
  using string_view = skyr::string_view;
  using value_type = std::pair<string_type, string_type>;
  using const_iterator = std::vector<value_type>::const_iterator;

  /// Constructor
  url_search_parameters();

  /// Constructor
  /// \param query
  explicit url_search_parameters(string_view query);

  /// \param name
  /// \param value
  void append(const string_type &name, const string_type &value);

  /// \param name
  void remove(const string_type &name);

  /// \param name
  /// \returns
  optional<string_type> get(const string_type &name) const noexcept;
  
  /// \param name
  /// \returns
  std::vector<string_type> get_all(const string_type &name) const;

  /// \param name
  /// \returns `true` if the value is in the search parameters, `false` otherwise.
  bool contains(const string_type &name) const noexcept;

  /// \param name
  /// \param value
  void set(const string_type &name, const string_type &value);

  ///
  void clear() noexcept;

  ///
  void sort();

  /// \returns
  const_iterator begin() const noexcept;

  /// \returns
  const_iterator end() const noexcept;

  /// \returns
  string_type to_string() const;

 private:
  void update();

  std::vector<value_type> parameters_;
};
}  // namespace skyr

#endif  // SKYR_URL_SEARCH_PARAMETERS_INC
