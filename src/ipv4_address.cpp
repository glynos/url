// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <cmath>
#include <locale>
#include <vector>
#include <skyr/optional.hpp>
#include "skyr/ipv4_address.hpp"

namespace skyr {
namespace {
optional<std::uint64_t> parse_ipv4_number(
    string_view input,
    bool &validation_error_flag) {
  auto R = 10;

  if ((input.size() >= 2) && (input[0] == '0') && (std::tolower(input[1], std::locale("C")) == 'x')) {
    input = input.substr(2);
    R = 16;
  }
  else if ((input.size() >= 2) && (input[0] == '0')) {
    input = input.substr(1);
    R = 8;
  }

  if (input.empty()) {
    return 0;
  }

  try {
    auto number = std::stoul(input.to_string(), nullptr, R);
    return number;
  }
  catch (std::exception &) {
    return nullopt;
  }
}
}  // namespace

std::tuple<bool, optional<ipv4_address>> parse_ipv4_address(string_view input) {
  auto validation_error_flag = false;

  std::vector<std::string> parts;
  parts.push_back(std::string());
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
    return {true, nullopt};
  }

  auto numbers = std::vector<std::uint64_t>();

  for (const auto &part : parts) {
    if (part.empty()) {
      return {true, nullopt};
    }

    auto number = parse_ipv4_number(string_view(part), validation_error_flag);
    if (!number) {
      return {true, nullopt};
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
    return {false, nullopt};
  }

  if (numbers.back() >=
      static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
    // validation_error = true;
    return {false, nullopt};
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto number : numbers) {
    ipv4 += number * std::pow(256, 3 - counter);
    ++counter;
  }

  return {true, ipv4_address(ipv4)};
}
}  // namespace skyr
