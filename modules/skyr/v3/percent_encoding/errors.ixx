// Copyright 2019-21 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;
export module skyr.v3.percent_encoding.errors;

export {
  namespace skyr::inline v3::percent_encoding {
    /// Enumerates percent encoding errors
    enum class percent_encode_errc {
      /// Input was not a hex value
      non_hex_input,
      /// Overflow
      overflow,
    };
  }  // namespace skyr::inline v3::percent_encoding
}  // export
