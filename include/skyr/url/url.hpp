// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2018 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_WHATWG_URL_INC
#define NETWORK_URI_WHATWG_URL_INC

/**
 * \file
 * \brief Contains the url class.
 */

#include <skyr/url/url_parse.hpp>

#ifdef NETWORK_URI_MSVC
#pragma warning(push)
#pragma warning(disable : 4251 4231 4660)
#endif


namespace skyr {
  class type_error : public std::runtime_error {
   public:

    type_error() : std::runtime_error("Type error") {}
  };

  class url {
   public:

    explicit url(const std::string &input);
    url(const std::string &input, const std::string &base);

    std::string href() const;
    std::string origin() const;
    std::string protocol() const;
    std::string username() const;
    std::string password() const;
    std::string host() const;
    std::string hostname() const;
    std::string port() const;
    std::string pathname() const;
    std::string search() const;
    std::string hash() const;

   private:

    url_record url_;
    // query object

  };
} // namespace skyr

#endif  // NETWORK_URI_WHATWG_URL_INC
