// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef URI_PATH_ITERATOR_HPP
#define URI_PATH_ITERATOR_HPP

#include <skyr/optional.hpp>
#include <skyr/string_view.hpp>

namespace skyr {
/**
 * \class url_path_iterator
 */
class url_path_iterator {
 public:

  using value_type = skyr::string_view;
  using difference_type = ptrdiff_t;
  using pointer = const value_type *;
  using reference = const value_type &;
  using iterator_category = std::forward_iterator_tag;

  url_path_iterator() = default;

  explicit url_path_iterator(optional<skyr::string_view> path)
      : path_{path}, element_{} {
    if (path_ && path_.value().empty()) {
      path_ = nullopt;
    } else {
      assign();
    }
  }

  url_path_iterator(const url_path_iterator &) = default;

  url_path_iterator &operator=(const url_path_iterator &) = default;

  reference operator++() noexcept {
    increment();
    return element_;
  }

  value_type operator++(int) noexcept {
    auto original = element_;
    increment();
    return original;
  }

  reference operator*() const noexcept {
    return element_;
  }

  pointer operator->() const noexcept {
    return addressof(element_);
  }

  bool operator==(const url_path_iterator &other) const noexcept {
    if (!path_ && !other.path_) {
      return true;
    }
    else if (path_ && other.path_) {
      // since we're comparing substrings, the address of the first
      // element in each iterator must be the same
      return addressof(element_) == addressof(other.element_);
    }
    return false;
  }

  inline bool operator!=(const url_path_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void advance() noexcept {
    assert(path_ && "Invalid path");

    if (!path_.value().empty()) {
      auto first = std::begin(path_.value()), last = std::end(path_.value());

      auto sep_it =
          std::find_if(first, last, [](char c) -> bool {
            return (c == '/') || (c == '\\');
          });

      if (sep_it != last) {
        ++sep_it;  // skip next separator
      }

      // reassign path to the next element
      path_ = skyr::string_view(std::addressof(*sep_it), std::distance(sep_it, last));
    }
    else {
      path_ = nullopt;
    }
  }



  void assign() noexcept {
    if (path_) {
      auto first = std::begin(path_.value()), last = std::end(path_.value());

      auto sep_it =
          std::find_if(first, last, [](char c) -> bool {
            return (c == '/') || (c == '\\');
          });

      element_ =
          skyr::string_view(std::addressof(*first), std::distance(first, sep_it));
    }
  }

  void increment() noexcept {
    advance();
    assign();
  }

  optional<skyr::string_view> path_;
  value_type element_;
};
}  // namespace skyr

#endif //URI_PATH_ITERATOR_HPP
