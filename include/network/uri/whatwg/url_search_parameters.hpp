// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_URL_SEARCH_PARAMETERS_INC
#define NETWORK_URI_URL_SEARCH_PARAMETERS_INC

#include <string>
#include <vector>
#include <network/optional.hpp>

namespace network {
namespace whatwg {
/**
 * \class uri_search_parameters
 */
class url_search_parameters {
 public:
  /**
   * \brief
   */
  using string_type = std::string;
  
  /**
   * \brief
   */
  using value_type = std::pair<string_type, string_type>;
  
  /**
   * \brief
   */
  using const_iterator = std::vector<value_type>::const_iterator;

  /**
   * \brief Constructor.
   */
  url_search_parameters();

  /**
   * \brief Constructor.
   */
  explicit url_search_parameters(const string_type &query);

  /**
   * \brief Constructor.
   */
  url_search_parameters(std::initializer_list<value_type> parameters);

  /**
   * \brief
   */
  void append(const string_type &name, const string_type &value);

  /**
   * \brief
   */
  void remove(const string_type &name);

  /**
   * \brief
   */
  optional<string_type> get(const string_type &name) const noexcept;

  /**
   * \brief
   */
  bool contains(const string_type &name) const noexcept;

  /**
   * \brief
   */
  void set(const string_type &name, const string_type &value);

  /**
   * \brief
   */
  const_iterator begin() const noexcept;

  /**
   * \brief
   */
  const_iterator end() const noexcept;

  /**
   * \brief
   */
  string_type to_string() const;

  /**
   * \brief
   */
  void sort();

 private:
  void update();

  std::vector<value_type> parameters_;
};
}  // namespace whatwg
}  // namespace network

#endif  // NETWORK_URI_URL_SEARCH_PARAMETERS_INC
