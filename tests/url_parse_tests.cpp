// Copyright 2018-19 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <skyr/url/url_parse.hpp>

// http://formvalidation.io/validators/uri/

namespace {
class UrlGenerator : public Catch::Generators::IGenerator<std::string> {
 public:
  explicit UrlGenerator(const std::string &filename)
      : ifs_(filename) {
    if (!ifs_) {
      throw std::runtime_error("Unable to open file: " + filename);
    }

    next();
  }

  const std::string &get() const override {
    return current_url_;
  }

  bool next() override {
    std::getline(ifs_, current_url_);
    while ((current_url_.front() == '#') && ifs_) {
      std::getline(ifs_, current_url_);
    }
    return static_cast<bool>(ifs_);
  }

 private:
  std::ifstream ifs_;
  std::string current_url_;
};

Catch::Generators::GeneratorWrapper<std::string> url(const std::string &filename) {
  return Catch::Generators::GeneratorWrapper<std::string>(
      std::unique_ptr<Catch::Generators::IGenerator<std::string>>(
          new UrlGenerator(filename)));
}
}  // namespace


TEST_CASE("valid urls", "[url_parse]") {
  auto input = GENERATE(url("valid_urls.txt"));

  SECTION("test_valid_urls") {
    auto result = skyr::parse(input);
    CHECK(result);
  }
}
