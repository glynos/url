// Copyright (c) Glyn Matthews 2017-18.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URL_INC
#define NETWORK_URL_INC

/**
 * \file \brief Contains an implementation of the WhatWG URL
 *       specification.
 */

#include <network/uri/whatwg/url.hpp>
#include <network/uri/whatwg/url_search_parameters.hpp>

namespace network {
using url = whatwg::url;
using url_syntax_error = uri_syntax_error;
using url_search_parameters = whatwg::url_search_parameters;

/**
 * \brief \c url factory function.
 * \param first The first element in a string sequence.
 * \param last The end + 1th element in a string sequence.
 * \param ec Error code set if the sequence is not a valid URL.
 */
template <class InputIter>
inline url make_url(InputIter first, InputIter last, std::error_code &ec) {
  return whatwg::make_url(first, last, ec);
}

/**
 * \brief \c url factory function.
 * \param source A source string that is to be parsed as a URL.
 * \param ec Error code set if the source is not a valid URL.
 */
template <class Source>
inline url make_url(const Source &source, std::error_code &ec) {
  return whatwg::make_url(source, ec);
}

}  // namespace network

#endif  // NETWORK_URL_INC
