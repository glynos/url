// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_FILESYSTEM_PATH_HPP
#define SKYR_FILESYSTEM_PATH_HPP

#include <system_error>
#include <filesystem>
#include <skyr/expected.hpp>
#include <skyr/url.hpp>

namespace skyr {
namespace filesystem {
enum class path_errc {
  empty_path = 1,
  invalid_path,
};

std::error_code make_error_code(path_errc error);

expected<url, std::error_code> from_path(const std::filesystem::path &path);

expected<std::filesystem::path, std::error_code> to_path(const url &input);
}  // namespace filesystem
}  // namespace skyr

#endif  // SKYR_FILESYSTEM_PATH_HPP
