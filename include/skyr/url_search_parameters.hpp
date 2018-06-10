// Copyright 2017-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_URL_SEARCH_PARAMETERS_INC
#define NETWORK_URI_URL_SEARCH_PARAMETERS_INC

#include <string>
#include <vector>
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
/**
 * @class uri_search_parameters
 */
class url_search_parameters {
 public:
  /**
   * @brief
   */
  using string_type = std::string;

  /**
   * @brief
   */
  using string_view = skyr::string_view;
  
  /**
   * @brief
   */
  using value_type = std::pair<string_type, string_type>;
  
  /**
   * @brief
   */
  using const_iterator = std::vector<value_type>::const_iterator;

  /**
   * @brief Constructor.
   */
  url_search_parameters();

  /**
   * @brief Constructor.
   */
  explicit url_search_parameters(string_view query);

  /**
   * @brief Constructor.
   */
  url_search_parameters(std::initializer_list<value_type> parameters);

  /**
   * @brief
   */
  void append(const string_type &name, const string_type &value);

  /**
   * @brief
   */
  void remove(const string_type &name);

  /**
   * @brief
   */
  optional<string_type> get(const string_type &name) const noexcept;
  
  /**
   * @brief
   */
  std::vector<string_type> get_all(const string_type &name) const;

  /**
   * @brief
   */
  bool contains(const string_type &name) const noexcept;

  /**
   * @brief
   */
  void set(const string_type &name, const string_type &value);

  /**
   * @brief
   */
  void sort();

  /**
   * @brief
   */
  const_iterator begin() const noexcept;

  /**
   * @brief
   */
  const_iterator end() const noexcept;

  /**
   * @brief
   */
  string_type to_string() const;

 private:
  void update();

  std::vector<value_type> parameters_;
};
}  // namespace skyr

#endif  // NETWORK_URI_URL_SEARCH_PARAMETERS_INC
