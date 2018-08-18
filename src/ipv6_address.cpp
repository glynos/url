// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <vector>
#include <cassert>
#include <cstring>
#include <locale>
#include "skyr/optional.hpp"
#include "skyr/ipv6_address.hpp"

namespace skyr {
namespace {
class ipv6_address_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *ipv6_address_error_category::name() const noexcept {
  return "domain";
}

std::string ipv6_address_error_category::message(int error) const noexcept {
  switch (static_cast<ipv6_address_errc>(error)) {
    case ipv6_address_errc::validation_error:
      return "Validation error";
    default:
      return "(Unknown error)";
  }
}

static const ipv6_address_error_category category{};
}  // namespace

std::error_code make_error_code(ipv6_address_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
bool starts_with(
    string_view::const_iterator first,
    string_view::const_iterator last,
    const char *chars) noexcept {
  auto chars_first = chars, chars_last = chars + std::strlen(chars);
  auto chars_it = chars_first;
  auto it = first;
  while (chars_it != chars_last) {
    if (*it != *chars_it) {
      return false;
    }

    ++it;
    ++chars_it;

    if (it == last) {
      return (chars_it == chars_last);
    }
  }

  return true;
}

inline std::uint16_t hex_to_dec(char c) noexcept {
  assert(std::isxdigit(c, std::locale::classic()));

  auto c_lower = std::tolower(c, std::locale::classic());

  if (std::isdigit(c_lower, std::locale::classic())) {
    return static_cast<std::uint16_t>(c_lower - '0');
  }

  return static_cast<std::uint16_t>(c_lower - 'a') + 10;
}
}  // namespace

std::string ipv6_address::to_string() const {
  auto output = std::string();
  auto compress = skyr::optional<size_t>();

  auto sequences = std::vector<std::pair<size_t, size_t>>();
  auto in_sequence = false;

  auto first = std::begin(address_), last = std::end(address_);
  auto it = first;
  while (true) {
    if (*it == 0) {
      auto index = std::distance(first, it);

      if (!in_sequence) {
        sequences.emplace_back(index, 1);
        in_sequence = true;
      } else {
        ++sequences.back().second;
      }
    } else {
      if (in_sequence) {
        if (sequences.back().second == 1) {
          sequences.pop_back();
        }
        in_sequence = false;
      }
    }
    ++it;

    if (it == last) {
      if (!sequences.empty() && (sequences.back().second == 1)) {
        sequences.pop_back();
      }
      in_sequence = false;
      break;
    }
  }

  if (!sequences.empty()) {
    stable_sort(std::begin(sequences), std::end(sequences),
                [](const auto &lhs,
                   const auto &rhs) -> bool {
                  return lhs.second > rhs.second;
                });
    compress = sequences.front().first;
  }

  auto ignore0 = false;
  for (auto i = 0UL; i <= 7UL; ++i) {
    if (ignore0 && (address_[i] == 0)) {
      continue;
    } else if (ignore0) {
      ignore0 = false;
    }

    if (compress && (compress.value() == i)) {
      auto separator = (i == 0) ? std::string("::") : std::string(":");
      output += separator;
      ignore0 = true;
      continue;
    }

    std::ostringstream oss;
    oss << std::hex << address_[i];
    output += oss.str();

    if (i != 7) {
      output += ":";
    }
  }

  return output;
}

expected<ipv6_address, std::error_code> parse_ipv6_address(string_view input) {
  // auto validation_error = false;

  auto address = std::array<unsigned short, 8>{{0, 0, 0, 0, 0, 0, 0, 0}};
  auto piece_index = 0;
  auto compress = optional<decltype(piece_index)>();

  auto first = begin(input), last = end(input);
  auto it = first;

  if (*it == ':') {
    auto next_it = it;
    ++next_it;
    if (!starts_with(next_it, last, ":")) {
      // validation_error = true;
      return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
    }

    it += 2;
    ++piece_index;
    compress = piece_index;
  }

  while (it != last) {
    if (piece_index == 8) {
      // validation_error = true;
      return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
    }

    if (*it == ':') {
      if (compress) {
        // validation_error = true;
        return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
      }

      ++it;
      ++piece_index;
      compress = piece_index;
      continue;
    }

    auto value = 0;
    auto length = 0;

    while ((length < 4) && std::isxdigit(*it, std::locale::classic())) {
      value = value * 0x10 + hex_to_dec(*it);
      ++it;
      ++length;
    }

    if (*it == '.') {
      if (length == 0) {
        // validation_error = true;
        return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
      }

      it -= length;

      if (piece_index > 6) {
        // validation_error = true;
        return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
      }

      auto numbers_seen = 0;

      while (it != last) {
        auto ipv4_piece = optional<std::uint16_t>();

        if (numbers_seen > 0) {
          if ((*it == '.') && (numbers_seen < 4)) {
            ++it;
          } else {
            // validation_error = true;
            return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
          }
        }

        if (!std::isdigit(*it, std::locale::classic())) {
          // validation_error = true;
          return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
        }

        while (std::isdigit(*it, std::locale::classic())) {
          auto number = static_cast<std::uint16_t>(*it - '0');
          if (!ipv4_piece) {
            ipv4_piece = number;
          } else if (ipv4_piece.value() == 0) {
            // validation_error = true;
            return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
          } else {
            ipv4_piece = ipv4_piece.value() * std::uint16_t(10) + number;
          }

          if (ipv4_piece.value() > 255) {
            // validation_error = true;
            return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
          }

          ++it;
        }

        address[piece_index] = address[piece_index] * 0x100 + ipv4_piece.value();
        ++numbers_seen;

        if ((numbers_seen == 2) || (numbers_seen == 4)) {
          ++piece_index;
        }
      }

      if (numbers_seen != 4) {
        // validation_error = true;
        return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
      }

      break;
    } else if (*it == ':') {
      ++it;
      if (it == last) {
        // validation_error = true;
        return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
      }
    } else if (it != last) {
      // validation_error = true;
      return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
    }
    address[piece_index] = value;
    ++piece_index;
  }

  if (compress) {
    auto swaps = piece_index - compress.value();
    piece_index = 7;
    while ((piece_index != 0) && (swaps > 0)) {
      std::swap(address[piece_index], address[compress.value() + swaps - 1]);
      --piece_index;
      --swaps;
    }
  } else if (!compress && (piece_index != 8)) {
    // validation_error = true;
    return make_unexpected(make_error_code(ipv6_address_errc::validation_error));
  }

  return ipv6_address(address);
}
}  // namespace skyr
