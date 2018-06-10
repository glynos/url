// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "skyr/url_parse.hpp"
#include "skyr/url_record.hpp"
#include "url_schemes.hpp"
#include <sstream>
#include <cmath>

namespace skyr {
std::string ipv4_address::to_string() const {
  auto output = std::string();

  auto n = repr;

  for (auto i = 1U; i <= 4U; ++i) {
    std::ostringstream oss;
    oss << (n % 256);
    output = oss.str() + output;

    if (i != 4) {
      output = "." + output;
    }

    n = static_cast<std::uint32_t>(std::floor(n / 256.));
  }

  return output;
}

std::string ipv6_address::to_string() const {
  auto output = std::string();
  auto compress = optional<std::size_t>();

  auto sequences = std::vector<std::pair<std::size_t, std::size_t>>();
  auto in_sequence = false;

  auto first = std::begin(repr), last = std::end(repr);
  auto it = first;
  while (true) {
    if (*it == 0) {
      auto index = std::distance(first, it);

      if (!in_sequence) {
        sequences.emplace_back(index, 1);
        in_sequence = true;
      }
      else {
        ++sequences.back().second;
      }
    }
    else {
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
    std::stable_sort(std::begin(sequences), std::end(sequences),
                     [](const std::pair<std::size_t, std::size_t> &lhs,
                        const std::pair<std::size_t, std::size_t> &rhs) -> bool {
                       return lhs.second > rhs.second;
                     });
    compress = sequences.front().first;
  }

  auto ignore0 = false;
  for (auto i = 0UL; i <= 7UL; ++i) {
    if (ignore0 && (repr[i] == 0)) {
      continue;
    }
    else if (ignore0) {
      ignore0 = false;
    }

    if (compress && (compress.value() == i)) {
      auto separator = (i == 0)? std::string("::") : std::string(":");
      output += separator;
      ignore0 = true;
      continue;
    }

    std::ostringstream oss;
    oss << std::hex << repr[i];
    output += oss.str();

    if (i != 7) {
      output += ":";
    }
  }

  return std::string("[") + output + "]";
}

bool url_record::is_special() const {
  return skyr::details::is_special(basic_string_view<char>(scheme));
}

bool url_record::includes_credentials() const {
  return !username.empty() || !password.empty();
}
}  // namespace skyr
