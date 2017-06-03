// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2017 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_URI_INC
#define NETWORK_URI_URI_INC

/**
 * \file
 * \brief Contains the uri class.
 */

#include <network/uri/ietf/uri.hpp>

namespace network {
using uri = ietf::uri;
using uri_builder = ietf::uri_builder;

/**
 * \brief \c uri factory function.
 * \param first The first element in a string sequence.
 * \param last The end + 1th element in a string sequence.
 * \param ec Error code set if the sequence is not a valid URI.
 */
template <class InputIter>
inline uri make_uri(InputIter first, InputIter last, std::error_code &ec) {
  return ietf::make_uri(first, last, ec);
}

/**
 * \brief \c uri factory function.
 * \param source A source string that is to be parsed as a URI.
 * \param ec Error code set if the source is not a valid URI.
 */
template <class Source>
inline uri make_uri(const Source &source, std::error_code &ec) {
  return ietf::make_uri(source, ec);
}
}  // namespace network

#endif  // NETWORK_URI_URI_INC
