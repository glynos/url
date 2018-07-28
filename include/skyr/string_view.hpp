// Copyright 2016-18 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_STRING_VIEW_INC
#define SKYR_STRING_VIEW_INC

// simple test for G++ 6 if this compiler uses the C++14 or 17 standards
#if (__cplusplus == 201402L)
#define NETWORK_STRING_VIEW_STD14
#elif (__cplusplus == 201500L)
#define NETWORK_STRING_VIEW_STD17
#endif

#if defined(SKYR_STRING_VIEW_STD17)
#include <experimental/string_view>

namespace skyr {
using string_view = std::experimental::basic_string_view<char>;
}  // namespace skyr

#else

#include <string>
#include <iterator>
#include <cassert>
#include <stdexcept>
#include <algorithm>

namespace skyr {
/// An implementation of C++17 string_view (n3921)
template <class charT, class traits = std::char_traits<charT>>
class basic_string_view {
 public:
  typedef traits traits_type;
  typedef charT value_type;
  typedef charT* pointer;
  typedef const charT* const_pointer;
  typedef charT& reference;
  typedef const charT& const_reference;
  typedef const charT* const_iterator;
  typedef const_iterator iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef const_reverse_iterator reverse_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  static constexpr size_type npos = size_type(-1);

  /// Constructor.
  constexpr basic_string_view() noexcept : data_(nullptr), size_(0) {}

  /// Copy constructor.
  constexpr basic_string_view(const basic_string_view&) noexcept = default;

  /// Assignment operator.
  basic_string_view& operator=(const basic_string_view&) noexcept = default;

  /// Constructor.
  /// \param str
  template <class Allocator>
  basic_string_view(
      const std::basic_string<charT, traits, Allocator>& str) noexcept
      : data_(str.data()),
        size_(str.size()) {}

  /// Constructor.
  /// \param str
  constexpr basic_string_view(const charT* str)
      : data_(str), size_(traits::length(str)) {}

  /// Constructor.
  /// \param str
  /// \param len
  constexpr basic_string_view(const charT* str, size_type len)
      : data_(str), size_(len) {}

  /// \returns
  constexpr const_iterator begin() const noexcept { return data_; }

  /// \returns
  constexpr const_iterator end() const noexcept { return data_ + size_; }

  /// \returns
  constexpr const_iterator cbegin() const noexcept { return begin(); }

  /// \returns
  constexpr const_iterator cend() const noexcept { return end(); }

  /// \returns
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  /// \returns
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  /// \returns
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  /// \returns
  const_reverse_iterator crend() const noexcept { return rend(); }

  /// \returns
  constexpr size_type size() const noexcept { return size_; }

  /// \returns
  constexpr size_type length() const noexcept { return size_; }

  /// \returns
  constexpr size_type max_size() const noexcept { return size_; }

  /// \returns
  constexpr bool empty() const noexcept { return size_ == 0; }

  /// \params pos
  /// \returns
  constexpr const_reference operator[](size_type pos) const {
    return data_[pos];
  }

  /// \param pos
  /// \returns
  const_reference at(size_type pos) const {
    if (pos >= size_) {
      throw std::out_of_range("Index out of range.");
    }
    return data_[pos];
  }

  /// \returns
  const_reference front() const { return *begin(); }

  /// \returns
  const_reference back() const {
    auto last = (end()) - 1;
    return *last;
  }

  /// \returns
  constexpr const_pointer data() const noexcept { return data_; }

  void clear() noexcept {
    data_ = nullptr;
    size_ = 0;
  }

  /// \param n
  void remove_prefix(size_type n) {
    data_ += n;
    size_ -= n;
  }

  /// \param n
  void remove_suffix(size_type n) { size_ -= n; }

  /// \param s
  void swap(basic_string_view& s) noexcept {
    std::swap(data_, s.data_);
    std::swap(size_, s.size_);
  }

  /// \returns
  template <class Allocator>
  explicit operator std::basic_string<charT, traits, Allocator>() const {
    return to_string<Allocator>();
  }

  /// \param a
  /// \returns
  template <class Allocator = std::allocator<charT>>
  std::basic_string<charT, traits, Allocator> to_string(
      const Allocator& a = Allocator()) const {
    return std::basic_string<charT, traits, Allocator>(begin(), end(), a);
  }

  /// \param n
  /// \param pos
  /// \returns
  size_type copy(charT* s, size_type n, size_type pos = 0) const {
    size_type rlen = std::min(n, size() - pos);
    std::copy_n(begin() + pos, rlen, s);
    return rlen;
  }

  /// \param pos
  /// \param n
  /// \returns
  constexpr basic_string_view substr(size_type pos = 0,
                                     size_type n = npos) const {
    return basic_string_view(data() + pos, std::min(n, size() - pos));
  }

  /// \param s
  /// \returns
  constexpr int compare(basic_string_view s) const noexcept {
    return (size() < s.size())? -1 : ((size() > s.size())? 1 : traits::compare(data(), s.data(), size()));
  }

  /// \param pos1
  /// \param n1
  /// \param s
  /// \returns
  constexpr int compare(size_type pos1, size_type n1,
                        basic_string_view s) const {
    return substr(pos1, n1).compare(s);
  }

  /// \param pos1
  /// \param n1
  /// \param s
  /// \param pos2
  /// \param n2
  /// \returns
  constexpr int compare(size_type pos1, size_type n1, basic_string_view s,
                        size_type pos2, size_type n2) const {
    return substr(pos1, n1).compare(s.substr(pos2, n2));
  }

  /// \param s
  /// \returns
  constexpr int compare(const charT* s) const {
    return compare(basic_string_view(s));
  }

  /// \param pos1
  /// \param n1
  /// \param s
  constexpr int compare(size_type pos1, size_type n1, const charT* s) const {
    return substr(pos1, n1).compare(basic_string_view(s));
  }

  /// \param pos1
  /// \param n1
  /// \param s
  /// \param n2
  /// \returns
  constexpr int compare(size_type pos1, size_type n1, const charT* s,
                        size_type n2) const {
    return substr(pos1, n1).compare(basic_string_view(s, n2));
  }

 private:
  const_pointer data_;
  size_type size_;
};

/// Equality operator
/// \param lhs
/// \param rhs
/// \returns <tt>lhs.compare(rhs) == 0</tt>.
template <class charT, class traits>
constexpr bool operator==(basic_string_view<charT, traits> lhs,
                          basic_string_view<charT, traits> rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

/// Inequality operator.
/// \param lhs
/// \param rhs
/// \returns <tt>!(lhs == rhs)</tt>.
template <class charT, class traits>
constexpr bool operator!=(basic_string_view<charT, traits> lhs,
                          basic_string_view<charT, traits> rhs) noexcept {
  return !(lhs == rhs);
}

/// Comparison operator
/// \param lhs
/// \param rhs
/// \returns <tt>lhs.compare(rhs) < 0</tt>.
template <class charT, class traits>
constexpr bool operator<(basic_string_view<charT, traits> lhs,
                         basic_string_view<charT, traits> rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

/// Comparison operator
/// \param lhs
/// \param rhs
/// \returns <tt>rhs < lhs</tt>.
template <class charT, class traits>
constexpr bool operator>(basic_string_view<charT, traits> lhs,
                         basic_string_view<charT, traits> rhs) noexcept {
  return rhs < lhs;
}

/// Comparison operator
/// \param lhs
/// \param rhs
/// \returns <tt>!(lhs > rhs)</tt>.
template <class charT, class traits>
constexpr bool operator<=(basic_string_view<charT, traits> lhs,
                          basic_string_view<charT, traits> rhs) noexcept {
  return !(lhs > rhs);
}

/// Comparison operator
/// \param lhs
/// \param rhs
/// \returns <tt>!(lhs < rhs)</tt>.
template <class charT, class traits>
constexpr bool operator>=(basic_string_view<charT, traits> lhs,
                          basic_string_view<charT, traits> rhs) noexcept {
  return !(lhs < rhs);
}

/// Output stream operator
/// \param os
/// \param str
/// \returns
template <class charT, class traits>
std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>& os,
    basic_string_view<charT, traits> str) {
  return os << str.to_string();
}

///
typedef basic_string_view<char> string_view;
}  // namespace skyr

#endif  // defined(SKYR_STRING_VIEW_STD17)

#endif  // SKYR_STRING_VIEW_INC
