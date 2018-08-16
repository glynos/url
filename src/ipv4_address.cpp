// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <cmath>
#include <locale>
#include <vector>
#include <sstream>
#include <skyr/optional.hpp>
#include "skyr/ipv4_address.hpp"

namespace skyr {
namespace {
expected<std::uint64_t, ipv4_address_errc> parse_ipv4_number(
    string_view input,
    bool &validation_error_flag) {
  auto R = 10;

  if ((input.size() >= 2) && (input[0] == '0') && (std::tolower(input[1], std::locale::classic()) == 'x')) {
    input = input.substr(2);
    R = 16;
  } else if ((input.size() >= 2) && (input[0] == '0')) {
    input = input.substr(1);
    R = 8;
  }

  if (input.empty()) {
    return 0ULL;
  }

  try {
    auto pos = static_cast<std::size_t>(0);
    auto number = std::stoul(input.to_string(), &pos, R);
    if (pos != input.length()) {
      return make_unexpected(ipv4_address_errc::invalid_segment_number);
    }
    return number;
  }
  catch (std::exception &) {
    return make_unexpected(ipv4_address_errc::invalid_segment_number);
  }
}
}  // namespace

std::string ipv4_address::to_string() const {
  auto output = std::string();

  auto n = address_;

  for (auto i = 1U; i <= 4U; ++i) {
    output = std::to_string(n % 256) + output;

    if (i != 4) {
      output = "." + output;
    }

    n = static_cast<std::uint32_t>(std::floor(n / 256.));
  }

  return output;
}

expected<ipv4_address, ipv4_address_errc> parse_ipv4_address(string_view input) {
  auto validation_error_flag = false;

  std::vector<std::string> parts;
  parts.emplace_back();
  for (auto ch : input) {
    if (ch == '.') {
      parts.emplace_back();
    } else {
      parts.back().push_back(ch);
    }
  }

  if (parts.back().empty()) {
    validation_error_flag = true;
    if (parts.size() > 1) {
      parts.pop_back();
    }
  }

  if (parts.size() > 4) {
    return skyr::make_unexpected(ipv4_address_errc::more_than_4_segments);
  }

  auto numbers = std::vector<std::uint64_t>();

  for (const auto &part : parts) {
    if (part.empty()) {
      return skyr::make_unexpected(ipv4_address_errc::empty_part);
    }

    auto number = parse_ipv4_number(string_view(part), validation_error_flag);
    if (!number) {
      return skyr::make_unexpected(ipv4_address_errc::invalid_segment_number);
    }

    numbers.push_back(number.value());
  }

  if (validation_error_flag) {
    // validation_error = true;
  }

  auto numbers_first = begin(numbers), numbers_last = end(numbers);

  auto numbers_it =
      std::find_if(numbers_first, numbers_last,
                   [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last) {
    // validation_error = true;
  }

  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  numbers_it = std::find_if(numbers_first, numbers_last_but_one,
                            [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last_but_one) {
    return skyr::make_unexpected(ipv4_address_errc::validation_error);
  }

  if (numbers.back() >=
      static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
    // validation_error = true;
    return skyr::make_unexpected(ipv4_address_errc::validation_error);
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto number : numbers) {
    ipv4 += number * static_cast<std::uint64_t>(std::pow(256, 3 - counter));
    ++counter;
  }

  return {ipv4_address(static_cast<unsigned int>(ipv4))};
}
}  // namespace skyr
