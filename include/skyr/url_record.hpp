// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef URI_URL_RECORD_HPP
#define URI_URL_RECORD_HPP

#include <vector>
#include <iterator>
#include <string>
#include <cstdint>
#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
/**
 * @class url_record
 */
struct url_record {
  std::string url;

  std::string scheme;
  std::string username;
  std::string password;
  skyr::optional<std::string> host;
  skyr::optional<std::uint16_t> port;
  std::vector<std::string> path;
  skyr::optional<std::string> query;
  skyr::optional<std::string> fragment;

  bool cannot_be_a_base_url;
  skyr::optional<std::string> object;

  /**
   * @brief Constructor.
   */
  url_record()
      : url{}, cannot_be_a_base_url{false} {}

  /**
   *
   * @return
   */
  bool is_special() const;

  /**
   *
   * @return
   */
  bool includes_credentials() const;

};
}  // namespace skyr

#endif //URI_URL_RECORD_HPP
