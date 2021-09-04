// Copyright 2018-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <format>

export module skyr.v3.core.serialize;

export import skyr.v3.core.url_record;

namespace skyr::inline v3::details {
inline auto serialize_password(const url_record &url) -> std::string {
  return !url.password.empty() ? std::format(":{}", url.password) : std::string{};
}

inline auto serialize_credentials(const url_record &url) -> std::string {
  return url.includes_credentials() ? std::format("{}{}@", url.username, serialize_password(url)) : std::string{};
}

inline auto serialize_port(const url_record &url) -> std::string {
  return url.port ? std::format(":{}", url.port.value()) : std::string{};
}

inline auto serialize_file_scheme(const url_record &url) -> std::string {
  return (!url.host && (url.scheme == "file")) ? "//" : "";
}

inline auto serialize_authority(const url_record &url) -> std::string {
  return url.host
             ? std::format("//{}{}{}", serialize_credentials(url), url.host.value().serialize(), serialize_port(url))
             : serialize_file_scheme(url);
}

inline auto serialize_path(const std::vector<std::string> &path) -> std::string {
  std::string result{};
  for (auto &&element : path) {
    result += std::format("/{}", element);
  }
  return result;
}

inline auto serialize_path(const url_record &url) -> std::string {
  return url.cannot_be_a_base_url ? url.path.front() : serialize_path(url.path);
}

inline auto serialize_query(const url_record &url) -> std::string {
  return url.query ? std::format("?{}", url.query.value()) : std::string{};
}

inline auto serialize_fragment(const url_record &url) -> std::string {
  return url.fragment ? std::format("#{}", url.fragment.value()) : std::string{};
}
}  // namespace details

export {
  namespace skyr::inline v3 {
  /// Serializes a URL record according to the
  /// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
  ///
  /// \param url A URL record
  /// \returns A serialized URL string, excluding the fragment
  inline auto serialize_excluding_fragment(const url_record &url) -> url_record::string_type {
    return std::format("{}:{}{}{}", url.scheme, details::serialize_authority(url), details::serialize_path(url),
                       details::serialize_query(url));
  }

  /// Serializes a URL record according to the
  /// [WhatWG specification](https://url.spec.whatwg.org/#url-serializing)
  ///
  /// \param url A URL record
  /// \returns A serialized URL string
  inline auto serialize(const url_record &url) -> url_record::string_type {
    return std::format("{}{}", serialize_excluding_fragment(url), details::serialize_fragment(url));
  }
  }  // namespace skyr::inline v3
}
