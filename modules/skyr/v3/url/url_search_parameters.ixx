// Copyright 2017-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <optional>
#include <algorithm>
#include <cassert>
#include <format>

export module skyr.v3.url_search_parameters;

export import skyr.v3.core.parse_query;
import skyr.v3.percent_encoding.percent_encode;
import skyr.v3.percent_encoding.percent_decode;

export {
  namespace skyr::inline v3 {
  class url;

  namespace details {
  struct is_name {
    explicit is_name(std::string_view name) : name_(name) {
    }

    auto operator()(const query_parameter &parameter) noexcept {
      return name_ == parameter.name;
    }

    std::string_view name_;
  };
  }  // namespace details

  /// Supports iterating through
  /// [URL search parameters](https://url.spec.whatwg.org/#urlsearchparams)
  ///
  /// The API closely follows the
  /// [WhatWG IDL specification](https://url.spec.whatwg.org/#interface-urlsearchparams)
  class url_search_parameters {
    friend class url;

   public:
    /// string type
    /// \sa url::string_type
    using string_type = std::string;

    /// A key-value pair
    using value_type = query_parameter;

    /// An iterator through the search parameters
    using const_iterator = std::vector<value_type>::const_iterator;

    /// An alias to \c const_iterator
    using iterator = const_iterator;

    /// \c std::size_t
    using size_type = std::size_t;

    /// Default constructor
    url_search_parameters() = default;

    /// Constructor
    /// \param query The search string
    explicit url_search_parameters(std::string_view query) {
      initialize(query);
    }

    /// Constructor
    /// \param parameters
    explicit url_search_parameters(std::vector<query_parameter> parameters) : parameters_(std::move(parameters)) {
    }

    /// Constructor
    /// \param parameters
    url_search_parameters(std::initializer_list<value_type> parameters) : parameters_(parameters) {
    }

    ///
    /// \param other
    void swap(url_search_parameters &other) noexcept {
      std::swap(parameters_, other.parameters_);
    }

    /// Appends a name-value pair to the search string
    ///
    /// \param name The parameter name
    /// \param value The parameter value
    void append(std::string_view name, std::string_view value) {
      parameters_.emplace_back(std::string(name), std::string(value));
      //update();
    }

    /// Removes a parameter from the search string
    ///
    /// \param name The name of the parameter to remove
    void remove(std::string_view name) {
      auto first = std::begin(parameters_), last = std::end(parameters_);
      auto it = std::remove_if(first, last, details::is_name(name));
      parameters_.erase(it, last);
      //update();
    }

    /// \param name The search parameter name
    /// \returns The first search parameter value with the given name
    [[nodiscard]] auto get(std::string_view name) const -> std::optional<string_type> {
      auto first = std::cbegin(parameters_), last = std::cend(parameters_);
      auto it = std::find_if(first, last, details::is_name(name));
      return (it != last) ? it->value : std::nullopt;
    }

    /// \param name The search parameter name
    /// \returns All search parameter values with the given name
    [[nodiscard]] auto get_all(std::string_view name) const -> std::vector<string_type> {
      std::vector<string_type> result;
      result.reserve(parameters_.size());
      for (auto [parameter_name, value] : parameters_) {
        if (parameter_name == name) {
          result.emplace_back(value.value_or(""));
        }
      }
      return result;
    }

    /// Tests if there is a parameter with the given name
    ///
    /// \param name The search parameter name
    /// \returns `true` if the value is in the search parameters,
    /// `false` otherwise.
    [[nodiscard]] auto contains(std::string_view name) const noexcept -> bool {
      auto first = std::cbegin(parameters_), last = std::cend(parameters_);
      return std::find_if(first, last, details::is_name(name)) != last;
    }

    /// Sets a URL search parameter
    ///
    /// \param name The search parameter name
    /// \param value The search parameter value
    void set(std::string_view name, std::string_view value) {
      auto first = std::begin(parameters_), last = std::end(parameters_);
      auto it = std::find_if(first, last, details::is_name(name));
      if (it != last) {
        it->value = value;

        ++it;
        it = std::remove_if(it, last, details::is_name(name));
        parameters_.erase(it, last);
      } else {
        append(name, value);
      }
      //update();
    }

    /// Clears the search parameters
    ///
    /// \post `empty() == true`
    void clear() noexcept {
      parameters_.clear();
      //update();
    }

    /// Sorts the search parameters alphanumerically
    ///
    /// https://url.spec.whatwg.org/#example-searchparams-sort
    ///
    /// ```
    /// auto url = skyr::url(
    ///   "https://example.org/?q=\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88&key=e1f7bc78");
    /// url.search_parameters().sort();
    /// assert(url.search() == "?key=e1f7bc78&q=%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88");
    /// ```
    void sort() {
      static constexpr auto less_name = [](const auto &lhs, const auto &rhs) { return lhs.name < rhs.name; };

      auto first = std::begin(parameters_), last = std::end(parameters_);
      std::sort(first, last, less_name);
      //update();
    }

    /// \returns An iterator to the first element in the search parameters
    [[nodiscard]] auto cbegin() const noexcept {
      return parameters_.cbegin();
    }

    /// \returns An iterator to the last element in the search parameters
    [[nodiscard]] auto cend() const noexcept {
      return parameters_.cend();
    }

    /// \returns An iterator to the first element in the search parameters
    [[nodiscard]] auto begin() const noexcept {
      return cbegin();
    }

    /// \returns An iterator to the last element in the search parameters
    [[nodiscard]] auto end() const noexcept {
      return cend();
    }

    /// \returns `true` if the URL search string is empty, `false`
    ///          otherwise
    [[nodiscard]] auto empty() const noexcept {
      return parameters_.empty();
    }

    /// \returns The size of the parameters array (i.e. the
    ///          number of parameters)
    [[nodiscard]] auto size() const noexcept {
      return parameters_.size();
    }

    /// \returns The serialized URL search parameters
    [[nodiscard]] auto to_string() const -> string_type {
      auto result = string_type{};

      bool start = true;
      for (const auto &[name, value] : parameters_) {
        if (start) {
          result.append(percent_encoding::percent_encode(name));
          start = false;
        } else {
          result.append(std::format("&{}", percent_encoding::percent_encode(name)));
        }
        if (value) {
          result.append(std::format("={}", percent_encoding::percent_encode(value.value())));
        }
      }

      return result;
    }

   private:
    explicit url_search_parameters(url *url);

    void initialize(std::string_view query) {
      if (auto parameters = parse_query(query); parameters) {
        for (auto [name, value] : parameters.value()) {
          auto name_ = percent_encoding::percent_decode(name).value_or(std::string(name));
          auto value_ = value ? percent_encoding::percent_decode(value.value()).value_or(std::string(value.value())) : std::string();
          parameters_.emplace_back(name_, value_);
        }
      }
    }

    void update();

    std::vector<value_type> parameters_;
    url *url_ = nullptr;
  };

  ///
  /// \param lhs
  /// \param rhs
  inline void swap(url_search_parameters &lhs, url_search_parameters &rhs) noexcept {
    lhs.swap(rhs);
  }
  }  // namespace skyr::inline v3
}
