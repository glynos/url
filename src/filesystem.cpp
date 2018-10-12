// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/filesystem.hpp"
#include "skyr/percent_encode.hpp"

namespace skyr {
namespace filesystem {
namespace {
class path_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *path_error_category::name() const noexcept {
  return "filesystem path";
}

std::string path_error_category::message(int error) const noexcept {
  switch (static_cast<path_errc>(error)) {
    case path_errc::invalid_path:
      return "Invalid path";
    case path_errc::percent_decoding_error:
      return "Percent decoding error";
    default:
      return "(Unknown error)";
  }
}

static const path_error_category category{};
}  // namespace

std::error_code make_error_code(path_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

expected<url, std::error_code> from_path(const std::filesystem::path &path) {
  url result;
  if (!result.set_protocol("file")) {
    return make_unexpected(make_error_code(path_errc::invalid_path));
  }
  if (!result.set_pathname(path.string())) {
    return make_unexpected(make_error_code(path_errc::invalid_path));
  }
  return result;
}

expected<std::filesystem::path, std::error_code> to_path(const url &input) {
  auto pathname = input.pathname();
  auto decoded = percent_decode(pathname);
  if (!decoded) {
    return make_unexpected(make_error_code(path_errc::percent_decoding_error));
  }
  return std::filesystem::u8path(decoded.value());
}
}  // namespace filesystem
}  // namespace skyr
