// Copyright 2021 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <memory>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
import skyr.v3.containers;


struct test_destructor_call {
  bool *destructed = nullptr;

  explicit test_destructor_call(bool *destructed)
  : destructed(destructed) {
    *destructed = false;
  }

  test_destructor_call(const test_destructor_call &) = delete;
  auto operator=(const test_destructor_call &) -> test_destructor_call & = delete;
  test_destructor_call(test_destructor_call &&) = delete;
  auto operator=(test_destructor_call &&) -> test_destructor_call & = delete;

  ~test_destructor_call() {
    *destructed = true;
  }
};

TEST_CASE("pop back calls destructor", "[containers]") {
  auto destructed = false;

  auto vector = skyr::static_vector<std::shared_ptr<test_destructor_call>, 8>{};
  vector.emplace_back(std::make_shared<test_destructor_call>(&destructed));
  CHECK_FALSE(destructed);
  vector.pop_back();
  CHECK(destructed);
}

TEST_CASE("clear calls destructor", "[containers]") {
  auto destructed = false;

  auto vector = skyr::static_vector<std::shared_ptr<test_destructor_call>, 8>{};
  vector.emplace_back(std::make_shared<test_destructor_call>(&destructed));
  CHECK_FALSE(destructed);
  vector.clear();
  CHECK(destructed);
}

TEST_CASE("destructor calls element destructors", "[containers]") {
  auto destructed = false;

  {
    auto vector = skyr::static_vector<std::shared_ptr<test_destructor_call>, 8>{};
    vector.emplace_back(std::make_shared<test_destructor_call>(&destructed));
    CHECK_FALSE(destructed);
  }
  CHECK(destructed);
}
